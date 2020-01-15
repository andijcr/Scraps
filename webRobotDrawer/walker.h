#include <math.h>

class Walker{

public: 
    int dx_half = 400;
    int sx_half = 500;

private:

    Servo sxservo, dxservo;
    float time_action;
    bool acting {false};
    long last_time;

    static constexpr float maxvel = 0.0021941282;              //cm * ms^-1
    static constexpr float da_half = 0.153 / 2;               //(distance between wheels)/2,  cm
    static constexpr float compensation=1.19;           //wut?
    static constexpr float r_maxvel=(da_half*compensation)/maxvel;    //inverse of angular velocity of the spinning craft, ms * rad^-1

    int servoMap_us(float val, int half_excursion){
        return int(val * half_excursion) + 1500;
    }

    void sx(float val){
        sxservo.writeMicroseconds(servoMap_us(-val, sx_half));
    }

    void dx(float val){
        dxservo.writeMicroseconds(servoMap_us(val, dx_half));
    }

    void setTimeAction(float t){
        time_action=t;
        last_time=millis();
        acting=true;
    }
public:

    Walker(int Sx, int Dx) {
        dxservo.attach(Dx);
        sxservo.attach(Sx);
        stop();
    }

    bool update(){
        if(acting && millis()-last_time >= (time_action)){
            acting=false; stop();
        }
        return acting;
    }

    void rotate(float angle){
        if(angle==0){stop(); return;}

        setTimeAction(fabs(angle*r_maxvel));

        if(angle>0){
            sx(1);
            dx(-1);
        }else{
            sx(-1);
            dx(1);
        }
    }

    /* 
     * a positive r is from the center towards the sx wheel, a negative one is in the direction of the dx wheel
     * a 0 r correspond to rotation in place (rotate), a infinite r is a straight move
     *                   ____ graphically:____   
     *                   |  |      ↥      |  |
     * pos(r)++++++++++++|sx¦+++++ O -----¦dx|-----neg(r)
     *                   |  |             |  |
     *                   ‾‾‾‾             ‾‾‾‾  
     */
    void rotateArc(float angle, float r){
        if(r==0){ rotate(angle); return; }
        if(angle==0){ return; }
        if(isinf(r)){ return ;}

        float sign= angle>0 ? 1 : -1;
        float ms_tosweep= fabs(angle) * (fabs(r)+da_half) / maxvel;
        if(r>0){
            float sx_div_dx = (r - da_half )/(r+da_half);
            float dx_vel = sign;
            float sx_vel = dx_vel * sx_div_dx;
            setTimeAction(ms_tosweep);
            dx(dx_vel);
            sx(sx_vel);
        }else{
            float dx_div_sx = (r + da_half)/(r - da_half);
            float sx_vel = -sign;
            float dx_vel = sx_vel * dx_div_sx;
            sx(sx_vel);
            dx(dx_vel);
        }

    }


    void move(float mag){
        if(mag==0){stop(); return;}

        setTimeAction(fabs(mag/maxvel));

        if(mag>0){
            sx(1);
            dx(1);
        }else{
            sx(-1);
            dx(-1);
        }
    }


    void stop(){
        sx(0);
        dx(0);
    }
};
