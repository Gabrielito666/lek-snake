# lek-snake

I'm learning c, so I built this little app.

It's a snake for terminal. You can clone and play it by running the ./lek-snake binary directly from the terminal. It doesn't use the arrow keys, but ‘wasd’.

If you have ubuntu or any distribution that uses .bashrc you can run the following command to install the game.

```bash
mkdir $HOME/.lek-snake
curl -Lo $HOME/.lek-snake/lek-snake https://raw.githubusercontent.com/Gabrielito666/lek-snake/main/lek-snake && chmod +x $HOME/.lek-snake/lek-snake && echo "alias lek-snake='$HOME/.lek-snake/lek-snake'" >> "$HOME/.bashrc" && source $HOME/.bashrc && echo lek-snake is installed!
```

and then you can run it with:

```bash
lek-snake
```

otherwise you will have to clone the repo and run it directly.

```bash
git clone https://github.com/Gabrielito666/lek-snake.git && cd lek-snake && chmod +x ./lek-snake
```
```bash
./lek-snake
```