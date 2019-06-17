# i0sh

> i0Ek3's Shell. 

This is a light Shell which I create it to learn how a real Shell to work, and I dubbed it to `i0sh`.

## Screenshot

![](https://github.com/i0Ek3/i0sh/blob/master/pic/screenshot.jpg)

## Structure

- bin
    - i0sh `You can execute this binary file directly.`
- src
    - i0sh.c `Source code for i0sh.`

## Process

- Accept the commands from user input.
- Call fork() system call to create a child process.
- Parse and execute commands from input.
- Return back to step 1.

## Feature

- Basic commands support, but fewer, add later.
- Current workplace directory support.
- Error checking.
- Simple signal handler, like Ctrl-C.

## Problem

- \ and whitespace unsupport.
- Pipe and redirection unsupport.
- No parameter to reffered.
- Fewer builtin commands support.

## To-do

- Update this Shell to be feature-riched, like fish-shell? Maybe...
- There are more things to do...This is not the end.
