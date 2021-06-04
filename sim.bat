@echo off

REM create file to hold the analysis variables for each simulation
(	
	echo train_corrcoef,test_corrcoef, train_rmse, test_rmse
)>script_out.csv

for /l %%x in (1, 1, 10) do (
	REM Redefine fixed_point parameters
	(	
		echo /*******************************************
		echo  * File : fp_params.h
		echo  * Author : F. Emre Yazici
		echo  * Creation Date : 27.03.2021
		echo  ********************************************/
		echo.
		echo #pragma once
		echo.
		echo #define FRACTIONAL_BITS 5
		echo #define INTEGER_BITS 2
		echo #define SIGN_BIT 1
	)>fp_params.h
	REM Redefine network parameters
	(	
		echo /*******************************************
		echo  * File : sim_params.h
		echo  * Author : F. Emre Yazici
		echo  * Creation Date : 27.03.2021
		echo  ********************************************/
		echo.
		echo #pragma once
		echo.
		echo #define SIM_N 500
		echo #define SIM_p (0.01 * %%x^)
		echo #define SIM_g 1.5
		echo #define SIM_alpha 1
		echo #define SIM_nsecs 500
		echo #define SIM_dt 0.1
		echo #define SIM_learn_every 2
	)>sim_params.h
	:: Build and execute
	msbuild Simulation.sln -v:q -p:warninglevel=0 -p:Configuration=Release -p:platform=x86 /m
	cd Release
	Simulation.exe
	REM rename output file for plots
	copy "%~dp0\Release\sim_results.csv" "%~dp0\sim_results%%x.csv"
	cd ..
	REM save sim analysis
	python process_out.py
)



REM set loopcount= 8
REM :loop
REM [Commands you want to repeat]
REM set /a loopcount=loopcount-1
REM if %loopcount%==0 goto exitloop
REM goto loop
REM :exitloop
