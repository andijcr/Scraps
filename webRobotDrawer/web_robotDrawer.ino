#include <Console.h>
#include <Servo.h>

#include "button.h"
#include "walker.h"
#include "svgexecuter.h"
#include "cmdinterface.h"

char* path = "M 5.3,17.0 C 0.93,11.23 18.24,-1.18 17.05,12.01 C 15.86,25.21 15.16,24.57 13.34,20.02 C 11.42,15.21 19.79,6.79 25.60,8.23 C 36.95,11.03 30.70,22.87 16.26,31.47 Z";

void setup(){

    Bridge.begin();
    Console.begin();

    Walker walker{9, 10};
    walker.stop();
    SvgExecuter svg{&walker, path};

    while (!Console);

    for(;;){
        Cmd cmd{Cmd::NONE};
        float addparam=0;

        if(Console.available()){
            String str_cmd = Console.readStringUntil('\n');
            if(str_cmd.length()==0) break;

            switch(str_cmd[0]){
                case 'm':
                    cmd=Cmd::MOVE

                    walker.move(Console.parseFloat());
                    Console.println("moving");
                    while(walker.update()){
                    }
                    Console.println();
                    break;
                case 'r':
                    break;
                case 'p':
                    Console.print("sx: "); Console.print(walker.sx_half);
                    Console.print(" dx: "); Console.println(walker.dx_half);
                    break;
                case 's':
                    sub = Console.readStringUntil(' ');
                    if(sub.length()==0){continue;}
                    if(sub[0]=='s'){
                        walker.sx_half = Console.parseInt();
                    }else if(sub[0]=='d'){
                        walker.dx_half = Console.parseInt();
                    }
                    Console.println("ok");
                    break;
                default:
                    Console.println(".");
                    break;
            }
            Console.find("\n");
        }
    }
}

void loop(){
}



