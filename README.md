# xjuliabg

julia set dynamic wallpaper of ubuntu

![](fig1.png)

![](fig2.png)

## install

```bash
mkdir build
cmake ..
make
make install
```

## usage
```
usage: 
  juliabg -f fps -v speed -k time_kernel -r
params:
  -f fps    a float, the refresh rate of the program, default 22.38 (which is equal to 55.95 / 2.5)
  -v speed  a float, the speed of rotating, default is 0.01
  -k kernel a float, the closer to zero, the faster not-so-beautiful frame runs over. default is 0.7
  -r        reverse the rotating
  -h        show help
```