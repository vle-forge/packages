
####
## Models
#### 
LotkaVolterra <- function(Time,State,Pars) 
{ 
	print(paste("time ", Time))
	alpha = Pars["alpha"]
	beta = Pars["beta"]
	gamma = Pars["gamma"]
	delta = Pars["delta"]
	X = State["X"]
	Y = State["Y"]

	#print(paste("alpha ",alpha));print(paste("beta ",beta));print(paste("gamma ",gamma));print(paste("delta ",delta))
	print(paste("Y ",Y))
	print(paste("X ",X))

	dX = X * (alpha - beta * Y);
	dY = Y * (gamma * X - delta);

	print(paste("dY ",dY))
	print(paste("dX ",dX))

	return(list(c(dX,dY)))
}


Seir <- function(Time,State,Pars)
{
	beta = Pars["beta"]
	gamma = Pars["gamma"]
	sigma = Pars["sigma"]
	nu = Pars["nu"]

	S = State["S"]
	E = State["E"]
	I = State["I"]
	R = State["R"]

	n = S+E+I+R;
	dS =  - beta*S*I/n - nu*S;
	dE =  beta*S*I/n - (sigma)*E;
	dI = sigma*E - (gamma) * I;
	dR = gamma*I  + nu*S;

	return(list(c(dS,dE,dI,dR)))
}

### 
# functions for comparison with vle extension
# see test.cpp
### 

test_euler_LotkaVolterra = function()
{
	require("deSolve")
	pars = c(alpha = 1.5, beta = 1, gamma = 1, delta = 3)
	yini = c(X = 10, Y = 5)
	times = seq(0,15,by=0.001)
	res = euler(func = LotkaVolterra, y = yini, parms = pars, times = times) 
}

test_euler_Seir = function()
{
	require("deSolve")
	pars = c(beta = 0.9, gamma= 0.2, sigma = 0.5, nu = 0.0)
	yini = c(S = 10, E = 1, I = 0, R = 0)
	times = seq(0,15,by=0.01)
	res = euler(func = Seir, y = yini, parms = pars, times = times) 
}


test_rk4_LotkaVolterra = function()
{
	require("deSolve")
	pars = c(alpha = 1.5, beta = 1, gamma = 1, delta = 3)
	yini = c(X = 10, Y = 5)
	times = seq(0,15,by=0.001)
	res = rk4(func = LotkaVolterra, y = yini, parms = pars, times = times) 
}

test_rk4_Seir = function()
{
	require("deSolve")
	pars = c(beta = 0.9, gamma= 0.2, sigma = 0.5, nu = 0.0)
	yini = c(S = 10, E = 1, I = 0, R = 0)
	times = seq(0,15,by=0.01)
	res = rk4(func = Seir, y = yini, parms = pars, times = times) 
}

