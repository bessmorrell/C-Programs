%% Program Summary
%for (i=1920; i<=2020; i++)
%   sum of abs|predicted pop @ year - data pop @ year|
%we will need: 
%   array of population data
%   array of prediction data [TOUT,YOUT]
%       exponential,logistic

%% Create population matrix from data, exponential model, and logistic model

%data
Data = load('USpop1920_2020.txt');

%exponential
Y0=111000;%enter Y0 depending on year
TSPAN = [1920,2020];%enter correct year range
[TOUT,YOUT] = ode45(@USPopulation_Exp, TSPAN, Y0);
Exp = [TOUT,YOUT];

%logistic
Y0=111000;%enter Y0 depending on year
TSPAN = [1920,2020];%enter correct year range
[TOUT,YOUT] = ode45(@USPopulation_Log, TSPAN, Y0);
Log = [TOUT,YOUT];

%% Summation

sumExp=0;
sumLog=0;

% Data vs. Exponential model
for i = 1:1:21
    sumExp = sumExp + abs(Data(1+(i-1)*5,2)-Exp(1+(i-1)*2,2));
end

% Data vs Logistic model
for i = 1:1:21
    sumLog = sumLog + abs(Data(1+(i-1)*5,2)-Log(1+(i-1)*2,2));
end

%compare
disp('sum(abs(Data-Exponential)) =')
disp(sumExp)
disp('sum(abs(Data-Logistic)) =')
disp(sumLog)

if sumExp < sumLog
    disp('Exponential model is better fit')
end
if sumLog < sumExp
    disp('Logistic model is better fit')
end
%% Population model functions

%Exponential
function dy1 = USPopulation_Exp(~,y)
k = 0.0093;
dy1 = k*y;
end

%Logistic
function dy2 = USPopulation_Log(~,y)
    k = 0.0139;
    Pm = 800000;
    dy2 = k*y*((Pm-y)/Pm);
end
