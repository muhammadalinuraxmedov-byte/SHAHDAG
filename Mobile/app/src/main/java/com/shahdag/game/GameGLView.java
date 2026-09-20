package com.shahdag.game;

import android.content.Context;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.opengl.Matrix;
import android.view.MotionEvent;
import android.graphics.RectF;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;
import java.nio.ShortBuffer;
import java.util.ArrayList;
import java.util.List;

public class GameGLView extends GLSurfaceView {
    final Renderer renderer;
    float d = 1f;
    public GameGLView(Context c) {
        super(c);
        setEGLContextClientVersion(2);
        renderer = new Renderer(c);
        setRenderer(renderer);
        setRenderMode(RENDERMODE_CONTINUOUSLY);
        setFocusable(true); setFocusableInTouchMode(true);
    }
    @Override public boolean onTouchEvent(MotionEvent e) { return renderer.onTouch(e); }

    static class Renderer implements GLSurfaceView.Renderer {
        final Context ctx;
        int program, aPos, aNormal, uMVP, uColor, uLightDir;
        FloatBuffer cubeV, cubeN; ShortBuffer cubeI;
        final float[] proj=new float[16], view=new float[16], vp=new float[16], model=new float[16], mvp=new float[16];
        float px=0f, py=1.65f, pz=6f, yaw=180f, pitch=0f;
        int vw=1, vh=1;
        float moveX=0, moveY=0; boolean firing=false; float lastTime;
        float lookX, lookY; boolean dragging=false;
        List<Box> boxes = new ArrayList<>(); List<Enemy> enemies = new ArrayList<>();
        int shots=0;

        Renderer(Context c){ ctx=c; buildScene(); }
        void buildScene(){
            // Street / yard / house / garage. Units are meters-ish.
            boxes.add(new Box(0,-0.15f,0, 18,0.3f,18, 0.13f,0.14f,0.15f));
            boxes.add(new Box(0,1.5f,-2.5f, 10,3,0.25f, 0.32f,0.31f,0.30f)); // south wall
            boxes.add(new Box(-5,1.5f,1.2f, 0.25f,3,7.2f, 0.34f,0.33f,0.31f));
            boxes.add(new Box(5,1.5f,1.2f, 0.25f,3,7.2f, 0.34f,0.33f,0.31f));
            boxes.add(new Box(0,3.0f,1.2f, 10,0.25f,7.2f, 0.28f,0.28f,0.29f));
            // Interior partitions
            boxes.add(new Box(-1.2f,1.4f,1.2f,0.15f,2.8f,4.8f,0.20f,0.20f,0.21f));
            boxes.add(new Box(1.6f,1.4f,1.9f,4.8f,2.8f,0.15f,0.22f,0.22f,0.22f));
            boxes.add(new Box(0.0f,1.4f,3.0f,10,2.8f,0.15f,0.22f,0.22f,0.22f));
            // Door leaves
            boxes.add(new Box(-1.0f,1.0f,-2.35f,0.9f,2.1f,0.15f,0.12f,0.08f,0.05f));
            boxes.add(new Box(4.3f,1.1f,-1.4f,0.12f,2.2f,1.2f,0.10f,0.08f,0.06f));
            // Garage on right
            boxes.add(new Box(7.0f,1.5f,1.0f,4.0f,3.0f,5.0f,0.18f,0.19f,0.20f));
            boxes.add(new Box(7.0f,3.0f,1.0f,4.0f,0.25f,5.0f,0.16f,0.17f,0.18f));
            // sidewalk / curb / street edges
            boxes.add(new Box(0,0.02f,-7.0f,18,0.08f,1.8f,0.11f,0.11f,0.12f));
            boxes.add(new Box(-8,0.02f,0,1.0f,0.08f,16,0.09f,0.09f,0.10f));
            // props
            boxes.add(new Box(-7,0.55f,3.2f,0.7f,1.1f,0.7f,0.06f,0.12f,0.07f));
            boxes.add(new Box(3.8f,0.7f,6.1f,1.2f,1.4f,1.2f,0.20f,0.12f,0.05f));
            enemies.add(new Enemy(2.6f,1.0f,-0.4f));
            enemies.add(new Enemy(-3.3f,1.0f,2.8f));
            enemies.add(new Enemy(0.8f,1.0f,4.3f));
        }

        @Override public void onSurfaceCreated(javax.microedition.khronos.egl.EGLConfig c){
            GLES20.glClearColor(0.02f,0.025f,0.03f,1f);
            GLES20.glEnable(GLES20.GL_DEPTH_TEST);
            GLES20.glEnable(GLES20.GL_CULL_FACE);
            program = makeProgram(VS,FS);
            aPos=GLES20.glGetAttribLocation(program,"aPos"); aNormal=GLES20.glGetAttribLocation(program,"aNormal");
            uMVP=GLES20.glGetUniformLocation(program,"uMVP"); uColor=GLES20.glGetUniformLocation(program,"uColor"); uLightDir=GLES20.glGetUniformLocation(program,"uLightDir");
            buildCube(); lastTime=System.nanoTime()/1e9f;
        }
        @Override public void onSurfaceChanged(javax.microedition.khronos.opengles.GL10 gl,int w,int h){
            vw=w; vh=h; GLES20.glViewport(0,0,w,h); float a=(float)w/h; Matrix.perspectiveM(proj,0,72f,a,0.1f,100f);
        }
        @Override public void onDrawFrame(javax.microedition.khronos.opengles.GL10 gl){
            float now=System.nanoTime()/1e9f; float dt=Math.min(0.033f,now-lastTime); lastTime=now;
            update(dt); render();
        }

        void update(float dt){
            float rad=(float)Math.toRadians(yaw); float fx=(float)Math.sin(rad), fz=(float)Math.cos(rad);
            float rx=fz, rz=-fx;
            float speed=2.8f;
            px += (fx*moveY + rx*moveX)*speed*dt; pz += (fz*moveY + rz*moveX)*speed*dt;
            px=Math.max(-8.3f,Math.min(8.3f,px)); pz=Math.max(-6.2f,Math.min(7.8f,pz));
            for(Enemy e: enemies) e.update(dt,px,py,pz);
            if(firing && !enemies.isEmpty()) fire();
            firing=false;
        }
        void fire(){
            firing=false; shots++; float sy=(float)Math.sin(Math.toRadians(yaw)), cy=(float)Math.cos(Math.toRadians(yaw));
            float dx=sy, dz=cy;
            for(Enemy e: enemies){
                float ex=e.x-px, ez=e.z-pz; float t=ex*dx+ez*dz; if(t<0||t>20) continue; float cx=px+dx*t, cz=pz+dz*t; float dist=(float)Math.hypot(e.x-cx,e.z-cz); if(dist<0.65f){ e.health--; break; }
            }
            for(int i=enemies.size()-1;i>=0;i--) if(enemies.get(i).health<=0) enemies.remove(i);
        }

        void render(){
            GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT|GLES20.GL_DEPTH_BUFFER_BIT); GLES20.glUseProgram(program);
            Matrix.setLookAtM(view,0,px,py,pz, px+(float)Math.sin(Math.toRadians(yaw))*10f, py+(float)Math.tan(Math.toRadians(-pitch))*3f, pz+(float)Math.cos(Math.toRadians(yaw))*10f,0,1,0);
            Matrix.multiplyMM(vp,0,proj,0,view,0); GLES20.glUniform3f(uLightDir,-0.25f,-1f,-0.4f);
            for(Box b: boxes) drawBox(b.x,b.y,b.z,b.sx,b.sy,b.sz,b.r,b.g,b.b);
            for(Enemy e: enemies) drawBox(e.x,1.0f,e.z,0.75f,2.0f,0.55f,0.46f,0.06f,0.04f);
            // simple weapon block in front of camera
            drawBox(px+(float)Math.sin(Math.toRadians(yaw))*0.65f, py-0.35f, pz+(float)Math.cos(Math.toRadians(yaw))*0.65f, 0.18f,0.16f,0.65f,0.07f,0.08f,0.09f);
        }
        void drawBox(float x,float y,float z,float sx,float sy,float sz,float r,float g,float b){
            Matrix.setIdentityM(model,0); Matrix.translateM(model,0,x,y,z); Matrix.scaleM(model,0,sx,sy,sz); Matrix.multiplyMM(mvp,0,vp,0,model,0);
            GLES20.glUniformMatrix4fv(uMVP,1,false,mvp,0); GLES20.glUniform3f(uColor,r,g,b);
            cubeV.position(0); GLES20.glEnableVertexAttribArray(aPos); GLES20.glVertexAttribPointer(aPos,3,GLES20.GL_FLOAT,false,0,cubeV);
            cubeN.position(0); GLES20.glEnableVertexAttribArray(aNormal); GLES20.glVertexAttribPointer(aNormal,3,GLES20.GL_FLOAT,false,0,cubeN);
            cubeI.position(0); GLES20.glDrawElements(GLES20.GL_TRIANGLES,36,GLES20.GL_UNSIGNED_SHORT,cubeI);
            GLES20.glDisableVertexAttribArray(aPos); GLES20.glDisableVertexAttribArray(aNormal);
        }

        void buildCube(){
            float[] v={-1,-1,-1, 1,-1,-1, 1,1,-1,-1,1,-1, -1,-1,1, 1,-1,1, 1,1,1,-1,1,1};
            float[] n={0,0,-1,0,0,-1,0,0,-1,0,0,-1, 0,0,1,0,0,1,0,0,1,0,0,1, -1,0,0,-1,0,0,-1,0,0,-1,0,0, 1,0,0,1,0,0,1,0,0,1,0,0, 0,-1,0,0,-1,0,0,-1,0,0,-1,0, 0,1,0,0,1,0,0,1,0,0,1,0};
            short[] idx={0,1,2,0,2,3, 4,6,5,4,7,6, 0,4,5,0,5,1, 1,5,6,1,6,2, 2,6,7,2,7,3, 4,0,3,4,3,7};
            cubeV=buf(v); cubeI=sbuf(idx); // normals are approximated per face using a longer buffer
            float[] nn=new float[36]; int k=0; for(int f=0;f<6;f++){float nx=f==0?0:f==1?0:f==2?-1:f==3?1:0; float ny=f==4?-1:f==5?1:0; float nz=f==0?-1:f==1?1:0; for(int i=0;i<4;i++){nn[k++]=nx;nn[k++]=ny;nn[k++]=nz;}} cubeN=buf(nn);
        }
        static FloatBuffer buf(float[] a){ByteBuffer b=ByteBuffer.allocateDirect(a.length*4).order(ByteOrder.nativeOrder()); FloatBuffer f=b.asFloatBuffer(); f.put(a).position(0); return f;}
        static ShortBuffer sbuf(short[] a){ByteBuffer b=ByteBuffer.allocateDirect(a.length*2).order(ByteOrder.nativeOrder()); ShortBuffer s=b.asShortBuffer(); s.put(a).position(0); return s;}
        static int makeShader(int type,String src){int s=GLES20.glCreateShader(type);GLES20.glShaderSource(s,src);GLES20.glCompileShader(s);return s;}
        static int makeProgram(String vs,String fs){int p=GLES20.glCreateProgram();GLES20.glAttachShader(p,makeShader(GLES20.GL_VERTEX_SHADER,vs));GLES20.glAttachShader(p,makeShader(GLES20.GL_FRAGMENT_SHADER,fs));GLES20.glLinkProgram(p);return p;}
        boolean onTouch(MotionEvent e){
            int a=e.getActionMasked(); float x=e.getX(), y=e.getY(); float w=vw,h=vh;
            if(a==MotionEvent.ACTION_DOWN||a==MotionEvent.ACTION_POINTER_DOWN){ if(x<w*0.42f && y>h*0.45f){ moveX=(x-w*0.2f)/(w*0.18f); moveY=(h*0.72f-y)/(h*0.2f); moveX=Math.max(-1,Math.min(1,moveX)); moveY=Math.max(-1,Math.min(1,moveY)); } else if(x>w*0.78f&&y>h*0.72f){ firing=true; } else { dragging=true; lookX=x; lookY=y; } return true; }
            if(a==MotionEvent.ACTION_MOVE){ if(x<w*0.55f&&y>h*0.45f){ moveX=(x-w*0.2f)/(w*0.18f); moveY=(h*0.72f-y)/(h*0.2f); moveX=Math.max(-1,Math.min(1,moveX)); moveY=Math.max(-1,Math.min(1,moveY)); } else if(dragging){ float dx=x-lookX,dy=y-lookY; yaw += dx*0.12f; pitch += dy*0.06f; pitch=Math.max(-45,Math.min(45,pitch)); lookX=x;lookY=y;} return true; }
            if(a==MotionEvent.ACTION_UP||a==MotionEvent.ACTION_CANCEL||a==MotionEvent.ACTION_POINTER_UP){ moveX=0;moveY=0;dragging=false; return true; } return true;
        }

        static class Box {float x,y,z,sx,sy,sz,r,g,b; Box(float X,float Y,float Z,float a,float bb,float c,float R,float G,float B){x=X;y=Y;z=Z;sx=a;sy=bb;sz=c;r=R;g=G;b=B;}}
        static class Enemy {float x,y,z;int health=3;float t; Enemy(float X,float Y,float Z){x=X;y=Y;z=Z;} void update(float dt,float px,float py,float pz){t+=dt; float dx=px-x,dz=pz-z; float d=(float)Math.hypot(dx,dz); if(d<7&&d>1.8){x+=(dx/d)*dt*0.35f;z+=(dz/d)*dt*0.35f;}}}
        static final String VS="attribute vec3 aPos; attribute vec3 aNormal; uniform mat4 uMVP; uniform vec3 uLightDir; varying float vL; void main(){ gl_Position=uMVP*vec4(aPos,1.0); float l=max(dot(normalize(aNormal),normalize(-uLightDir)),0.0); vL=0.25+0.75*l;}";
        static final String FS="precision mediump float; uniform vec3 uColor; varying float vL; void main(){ gl_FragColor=vec4(uColor*vL,1.0); }";
    }
}
