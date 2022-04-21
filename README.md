# Memory
This Memory game was designed by Terry Luan and Shauda Islam. It was created for the DE1-SoC Computer 
and was created using the [CPUlator Computer System Simulator](https://cpulator.01xz.net/)
<br/><br/><br/>


<p align="center" style="text-align:center">
  <img src="https://user-images.githubusercontent.com/44815845/164414375-ceda74be-238b-419c-9778-df26528d671a.png" width=250/>
  <img src="https://user-images.githubusercontent.com/44815845/164413845-0ad654b7-99ba-4881-9a08-1e249321eeb4.png" width=250/>
</p>

<p align="center">
  <img src="https://user-images.githubusercontent.com/44815845/164414087-c0721e9f-e487-4fd8-9aef-c993e58794e1.png" width=250/>
  <img src="https://user-images.githubusercontent.com/44815845/164413518-e5bbeb00-eea1-46bf-8352-7179f593851e.png" width=250/>
</p >
<h4 align="center"> Screenshots of the initial setup of the Memory Game </h4>

<br/><br/>
<p align="center" style="text-align:center">
  <img src="https://user-images.githubusercontent.com/44815845/164428800-ac9cecf3-a396-4bf3-8887-fedc9b1c3d08.png" />
</p>
<h4 align="center"> A Screenshot of a completed Memory Game board</h4>

# Setup Instructions
1. To run the Memory Game on the CPUlator Simulator, first go to https://cpulator.01xz.net/?sys=arm-de1soc. 
2. Change the Language from ARMv7 to C.
3. Then copy and paste [main.c](https://github.com/terryluan12/Memory/blob/master/main.c) into the main code window.<br/>
4. Replace `#include global.h` and `#include cards.h` with the contents of [global.h](https://github.com/terryluan12/Memory/blob/master/global.h) and [cards.h](https://github.com/terryluan12/Memory/blob/master/cards.h).
5. To play the game, you can press "Continue" on the top ribbon.

![image](https://user-images.githubusercontent.com/44815845/164425030-10b98a14-9311-4d44-afd1-219094751c6a.png)


# Playing the Game

Everything important will be found in the right panel of the screen.
The important aspects of the game are listed below:<br/>
**Push buttons** : Difficulty Selection (0 for easy, 1 for medium, 2 for hard, 3 for impossible) <br/>
**Seven-segment display** : High Score/number of moves<br/>
**VGA pixel buffer** : Game board<br/>
**PS/2 keyboard or mouse** : Input box

Once you click continue, make a level/difficulty selection by clicking on the push button
(it must be clicked on and off for the selection to be registered). 
The game should be loaded on the VGA buffer. From there, you can play by clicking into the input box! 
Flip the card by typing the number/letter into the input box, and match two cards of the same value. 


Once all cards have been matched, the cards will reset and you can restart the game by selecting the same, or a different difficulty.
Your score is based on amount of moves used. 
The high score is stored in the first 3 digits of the hex display, and your current score is stored in the last 3 digits of the hex display
