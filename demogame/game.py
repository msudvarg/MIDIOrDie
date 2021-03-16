#!/usr/bin/env python
import pygame
import pygame.midi

CHANNEL_1_NOTE_ON = 144
CHANNEL_1_NOTE_OFF = 128

MIDDLE_C = 60

pygame.init()
pygame.midi.init()
midistream = pygame.midi.Input(2)

width, height = 320, 240

speed = [2, 2]

black = (0, 0, 0)

screen = pygame.display.set_mode((width, height))

ball = pygame.image.load("intro_ball.gif")
ballrect = ball.get_rect()

keys = [False, False, False, False, False]

should_quit = False
while not should_quit:
  for event in pygame.event.get():
    if event.type == pygame.QUIT:
      should_quit = True

  while midistream.poll():
    event = midistream.read(1)[0][0]
    if event[0] == CHANNEL_1_NOTE_ON:
      if event[1] >= MIDDLE_C and event[1] < MIDDLE_C + 5:
        keys[event[1] - MIDDLE_C] = True
    if event[0] == CHANNEL_1_NOTE_OFF:
      if event[1] >= MIDDLE_C and event[1] < MIDDLE_C + 5:
        keys[event[1] - MIDDLE_C] = False
    print(keys)
  
  ballrect = ballrect.move(speed)
  if ballrect.left < 0 or ballrect.right > width:
    speed[0] = -speed[0]
  if ballrect.top < 0 or ballrect.bottom > height:
    speed[1] = -speed[1]

  screen.fill(black)
  
  screen.blit(ball, ballrect)
  pygame.display.flip()
  pygame.time.wait(int(1000 / 60))
