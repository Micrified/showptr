### Compiling

Compile with: `gcc -D_GNU_SOURCE -o showptr showptr.c`

### GDB Check

Compile with: `gcc -D_GNU_SOURCE -g -o showptr showptr.c`

1. Start gdb with: `gdb showptr`.
2. Add breakpoint on line 72: `break 72`.
3. [Optionally] Ask to show assembly at break points with: `display/i $pc`
4. Run with: `run`.
5. Continue with `continue`.

