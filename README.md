# GLStuff

This is code and notes taken when working through some of the OpenGL tutorials at [learnopengl.com](https://learnopengl.com/).
It has a lot of problems and probably shouldnt be used by anyone other than me, since this is mainly just for learning.

It uses:
- GLFW 3.3.9
- GLAD
- GLM
- stb_image

Just the dependencies used on the learnopengl tutorials.

Things that are here:
- Shape and shader utilities
- Camera management utilities
- Projectile class with some basic collision (AABB)
- Texture management class
- A lot of bloat which i will probably get around to cleaning up in future

Things I want to add:
- More precise collision, AABB is not very good at detecting *real* collisions if the shape isnt a perfect rectangle. 		
  Probably need to use more checks which will come after the AABB check to see if we actually collide.
- Jumping and falling, will probably be inside the Player class
- Gravity, for the above and for other stuff
- Player collision with walls and other objects. Also actually finishing the player class.
- Enemies, or entities other than the player
- Sprites and model loading, all that jazz
- Lighting - would be rlly rlly cool
- Probably more that i cant think of.

