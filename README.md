# Terminal-Wordle-Clone
A Wordle clone that runs directly in your Linux terminal, written entirely in C.
This also works on Windows 10 and Windows 11.

## How To Use
Clone the repository to your local machine, then run `make` to compile.\*
Within this directory, run `./wordle` to begin the game, or `./wordle.exe` on Windows.

\* Note: `gcc` and `make` are required to use the makefile. On Windows, follow the steps [here](https://code.visualstudio.com/docs/cpp/config-mingw) to install `gcc`. Make can be found [here](https://gnuwin32.sourceforge.net/packages/make.htm) or installed with Chocolatey. All makefile options except `make zip` will work on Windows.

You have a few options:
- `./wordle` plays a game of classic Wordle, with a word selected at random.
- `./wordle -d` is a unique daily puzzle for the given day. This does NOT align with the NY Times daily Wordle, and is instead chosen at random from `words.txt`.
- `./wordle -h` pulls up a help menu.
- `./wordle -r` displays the rules of the game.
- `./wordle -c <FILE>` loads a custom word bank into the game (more on that below).
- `./wordle -H` plays a game of classic Wordle (with a randomly selected word), but on hard mode.

## Custom Word Banks
This Wordle clone supports custom word banks with words of any length. To make your own word bank, create a text file in the following format:
```
<NUMBER OF LETTERS PER WORD>
<NUMBER OF VALID ANSWERS>
ANSWER 1
ANSWER 2
...
ANSWER N
WORD 1
WORD 2
...
WORD M
```

- The first line of the text file is a whole number, `L`, equal to the number of letters found in each word.
- The second line of the text file is another whole number, `N`, where `N` is less than or equal to the number of words in the word bank. This is **not** accounted for by the program and must be verified by the author of the word bank.
  - The first `N` words in the word bank will be what the game chooses from when it picks a target word for that round
  - The remaining `M` words comprise the rest of the valid guesses. If a player guesses a word not found in the active word bank, the game rejects it.
 
The folder `word_banks` contains two custom word banks of my own, a six letter and a seven letter wordle variant, alongside `words.txt`, the default word bank.
Check any of these files for any clarification on the word bank system.
