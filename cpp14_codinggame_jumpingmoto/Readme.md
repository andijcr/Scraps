from [coding game](www.codingame.com)

## Skynet: Chasm

### The Goal
The goal for your program is to make a motorbike jump across a gap and land on a platform then stop.

### Rules

The platform is located on the otherside of a gap in the road above a chasm. The lengths of the road, gap and platform are measured in cells.
 

At the start of the program, you are given:

    The variable road: the length of the road before the gap.
    The variable gap: the length of the gap.
    The variable platform: the length of the platform.

At the start of each game turn, the motorbike gives you:

    The variable speed: its current speed.
    The variable coordX: its position on the road.

The motorbike's initial position is coordX = 0. It always moves in a straight line. At the end of each turn, it moves forward a number of spaces equal to its speed (speed). For example, if coordX = 1 and speed = 3, coordX will be 4 at the next turn. The bike can start with any speed, including being at a stop.
 

Before the end of the turn, you must output one of the following commands:

    SPEED: increases the speed of the motorbike by 1 (+1 to the speed variable).
    SLOW: decreases the speed of the motorbike by 1 (-1 to the speed variable).
    JUMP: makes the motorbike jump.
    WAIT: does nothing (the motorbike keeps the same speed).

 
### Victory Conditions
The mission is a success if the bike comes at a stop on the landing platform.

### My solution
This solution implements a Depth-First search, computed at the start of the game when the position and the velocity of the motorbike are given
