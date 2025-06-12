# Engineering-Final
  This game has two separate parts to it; a mine detection game and a roulette wheel. 
Players will select the difficulty the want at the start and then when they're ready, 
they'll hit both buttons to start. Then depening on their difficulty, they will play 
between 1 and 6 games testing where the bombs are. If they manage to avoid all the bombs, 
they'll get sent to the roulette wheel where they'll choose a color based on the extreme
light level they choose and if it lands on their color, the board will flash bright lights
and play a happy tune but if it doesn't, it'll play a sad tune and fade out red. 


  For the inputs and outputs, the left and right button decrease and increase the difficulty
level at the start and hitting both will start the game. The touch sensors will be used to see 
which place they choose to test and will be triggered when they go below a value of 50 (just 
touching it should trigger it). Then once they've completed all their rounds, the roulette 
wheel will appear and players will choose by picking black (making the light sensor go below 
a value of 10) and red (making the light sensor go above 150). Then the roulette wheel will 
have a yellow led "spin" around the board and will randomly stop on a color). If it lands on 
the player's color, it will play a happy tune and flash green but if it doesn't, it will play 
a sad tune and flash no lights. 
