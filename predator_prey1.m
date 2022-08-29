%% ODE Solver
Y0=[5,10];

odefun = @(t,y) predatorprey(t,y);
[TOUT,YOUT] = ode45(odefun, tspan, Y0);
%TOUT is vector of time points
%YOUT is matrix of population at each time point

%% Plot Predator-Prey 
figure(1)
plot(TOUT,YOUT(:,1),'r',TOUT,YOUT(:,2),'b')
xlabel('Time Unit');
ylabel('Population');
title('Predatory-Prey Model');
legend('Prey', 'Predator');
box on

%% Plot X vs Y
figure(2)
plot(YOUT(:,1),YOUT(:,2))
title('x(t) versus y(t)')
xlabel('x(t)');
ylabel('y(t)');
hold on

Y0=[10,10];

odefun = @(t,y) predatorprey(t,y);
[TOUT,YOUT] = ode45(odefun, tspan, Y0);
plot(YOUT(:,1),YOUT(:,2))
hold on

Y0=[15,10];

TSPAN = [0,1000];
odefun = @(t,y) predatorprey(t,y);
[TOUT,YOUT] = ode45(odefun, tspan, Y0);
plot(YOUT(:,1),YOUT(:,2))
legend('Y0=[5,10]','Y0=[10,10]','Y0=[15,10]')
hold off
box on


%% Predator-Prey Function
function dy = predatorprey(~,y)

dy = zeros(2,1); %vector of 2 derivatives

a = 0.04;
b = 0.0005;
c = 0.02;
d = 0.1;

dy(1)=a*y(1)-b*y(1)*y(2);
dy(2)=c*y(1)*y(2) - d*y(2);

end