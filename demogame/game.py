#!/usr/bin/env python
import pygame
import pygame.midi

CHANNEL_1_NOTE_ON = 144
CHANNEL_1_NOTE_OFF = 128

E3 = 52

pygame.init()
pygame.midi.init()
midistream = pygame.midi.Input(pygame.midi.get_default_input_id())

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
      if event[1] >= E3 and event[1] < E3 + 5:
        keys[event[1] - E3] = True
    if event[0] == CHANNEL_1_NOTE_OFF:
      if event[1] >= E3 and event[1] < E3 + 5:
        keys[event[1] - E3] = False
  
  screen.fill(black)

  for i in range(5):
    if keys[i]:
      pygame.draw.rect(screen, (255, 0, 0), pygame.Rect(width / 5 * i, 0, width / 5, height))
  
  pygame.display.flip()
  pygame.time.wait(int(1000 / 60))
