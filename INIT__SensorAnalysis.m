T = readtable('ULTRASONIC_TEST2.xlsx');

%Load Cell data
x = T(:, 10);
y = T(:, 11);

%Ultrasonic Sensor data
s1 = T(:, 1);
s2 = T(:, 2);

x = table2array(x);
y = table2array(y);

s1 = table2array(s1);
s2 = table2array(s2);

linefit = polyfit(x,y,1);
y1 = polyval(linefit,x);
y1 = y1;

linefit = polyfit(s1,s2,1);
z = polyval(linefit,x);
z = z;

fitDegree1 = fit(x,y,'poly1')
fitDegree2 = fit(s1,s2,'poly1')

hold on; 

plot(x,y,'o');
plot(x,y1);

title("Load Cell Sensor Data")
xlabel("Actual weight (N)")
ylabel("Residual weight (N)")

hold off;


hold on; 

plot(s1,s2,'o');
plot(s1,z);

title("Ultrasonic Sensor Data")
xlabel("Actual distance (cm)")
ylabel("Measured distance (cm)")

hold off;