# Blackjack with Reinforcement Learning

Implementation of optimal policy learning for the game of Blackjack, based on the DeepMind x UCL Reinforcement Learning Course (2015).

## Overview

This project applies reinforcement learning techniques to solve a simplified version of Blackjack. The objective is to learn a policy that maximizes the expected reward through experience and policy evaluation methods.

## Key topics covered:

Policy evaluation and improvement
Monte Carlo control
Exploration vs. exploitation
Value function estimation

## What This Project Includes

A simple Blackjack environment
Implementation of reinforcement learning algorithms to evaluate and improve policy
Training loop using several methods
Visualization of the optimal policy
Large number of games played with the optimal policy to show the casinos edge.

## Implementation guidelines and certified solution

<img width="500" alt="Screenshot 2025-04-14 at 15 18 06" src="https://github.com/user-attachments/assets/2f5e89b6-d76b-49b5-82c1-d9b01c489c69" /><img width="500" alt="Screenshot 2025-04-15 at 10 58 29" src="https://github.com/user-attachments/assets/27c413cf-c5af-44a7-a70f-ad7f7d13f120" />


## Results

<img width="376" alt="image" src="https://github.com/user-attachments/assets/f09f4f1b-81ee-4654-9058-6575ce7d87fd" />

As we can see it is an almost perfect match with the solution given by UCL. Only one state-action pair was incorrectly learned, likely due to an insufficient number of training episodes.
