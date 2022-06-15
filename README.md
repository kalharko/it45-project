# **it45-project**
Adrien Burgun, Oscar Dewasmes
<br><br>

## Compile
At the project's root :  
```
mkdir build
cd build
cmake ..

make
```

## Run
In the `build` folder :  
```
./it45-projet <path to data folder> <Number of iterations> <cut_off>
```

For example : `./it45-projet ../Instances/100-10/ 8 200`

The `cut_off` value is best left between 150 and 300


## Résults
The best solution found is printed to the console.  
The agents time tables are stored in `edt.txt`.  
Log of the evolution of the solution can be found in `log.csv`.  
