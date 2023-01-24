%Programa para comunicar tiva con matlab y pueda leer los datos del ADC
%para graficarlos.
clc
clear all 
close all
%Restea todos los puertos
instrreset
tam = 4;
s = serial('COM8','BaudRate',115200);
s.Terminator = 'A'; % matlab a saber cuando se manda la cadena completa
%Abre el puerto serial 
 fopen(s);
 %Crear figura
figure(1)
title('Voltaje ADC');
xlabel('Tiempo');
ylabel('Voltaje');
axis([0 100 0 3.5])
grid off;
hold on;
    ylim([0 3.4]);
   xlim([0 300]);
   fprintf(s,'G');
  while true
      Leer = fscanf(s,"%s");% guardar la lectura en un string
   for muestras = 1:300
      for i = 1:24
               if(mod(i,4) == 0) %
                   Lectura(muestras,1:4) = Leer((i-3):i);
                   Valor(muestras) = str2num(Lectura(muestras,:));
                   Voltaje(muestras) = (Valor(muestras)*3.3)/4096;
                   subplot(6,1,(i/4))
                   plot(Voltaje);
                   
                   grid on;
                   drawnow
               end 
      end     
      
   end
  end