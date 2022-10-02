loadlibrary('.\x64\USB3000.dll','.\USB3000.h')
libfunctions USB3000 -full

clear all;

sample_rate=1e6;
sample_time=1e9/sample_rate;
data_duration=1;
points=data_duration*sample_rate*0.1;
range=10.24;

Data = zeros(1,16*points);
DataPtr = libpointer('singlePtr',Data);
calllib('USB3000','USB3OpenDevice',0)                      %打开采集卡，参数不用改。
calllib('USB3000','USB3ReadDevcieSN',0)
calllib('USB3000','USB3ReadDevcieModel',0)
for i=1:8
    calllib('USB3000','SetUSB3AiChanSel',0,i - 1 ,0)        % 默认开启8通道，先关闭8通道
    calllib('USB3000','SetUSB3AiRange',0,i - 1 ,range)
end
for i= 1:1
    calllib('USB3000','SetUSB3AiChanSel',0,i - 1 ,1)           %开启相应通道  这里开启通道0
    calllib('USB3000','SetUSB3AiRange',0,i - 1 ,range)
end
calllib('USB3000','SetUSB3AiSampleMode',0,0)                   %设置采集模式 设置为连续采集
calllib('USB3000','SetUSB3AiConnectType',0,1)                  %设置连接方式 这里为NRSE
calllib('USB3000','SetUSB3AiSampleRate',0,sample_time)              %采样率这只为100000ns  换算采样率为10k
calllib('USB3000','SetUSB3AiTrigSource',0,0)                   %设置触发方式 这里设置为软件触发 执行软件触发函数即可触发采集
calllib('USB3000','SetUSB3AiConvSource',0,0)                   %设置触发源 这里设置为AI内部触发源

% fig = uifigure; %创建UI
% ax = uiaxes(fig,'Position',[10 10 550 400]); %在UI里选择绘制信号位置

while(1)

calllib('USB3000','SetUSB3ClrAiFifo',0)                        %清空缓冲区
calllib('USB3000','SetUSB3AiSoftTrig',0)                       %执行软件触发函数 触发采集

calllib('USB3000','USB3GetAi',0,points,DataPtr,4000)            %从缓冲区获取点数  设置为执行一次获取10000点 建议是单通道 采样率的十分之一的点数
                                                               %将获取的点数给到指针DATAPTR中超时时间时间设置为4000ms
Data = get(DataPtr,'Value');                                   %连续采集就是连续不断执行USB3getAi函数 数据就是指针内的数据，0到9999是第一个开启的通道
pause(data_duration)                                                       %若是多通道采集，数据就是第一个10000是开启的第一个通道的。第二个10000数据是第二个开启的通道的
myData = zeros(1,points);
myData = Data(1:points);



%Decoding--------------------------------------------------------------------------------------------------------------------------

myData=myData-mean(myData); %去直流
y=lowpass(myData,1e4,sample_rate); %1K低通取载波
y=highpass(y,1500,sample_rate); %80Hz高通去工频



for i=1:points
    newarray=y(i:i+5000);
    if max(newarray)-min(newarray)<1e-3  %找到第一个码的初始点，找到相对平缓的一段信号作为解码初始
        startpoint=i;
        break
    end
end

y=y(startpoint:end);



diffy=diff(y);

% plot(diffy)

edge_height=2e-4;
skip=60;
initial_status=0;
reverse_point=1;
for i = 1:(length(y)-1)
    if abs(diffy(i))<edge_height
        x(i)=initial_status;
    elseif abs(diffy(i))>=edge_height && (i-reverse_point)<=skip
        x(i)=initial_status;
    else
        initial_status=1-initial_status;
         x(i)=initial_status;
         reverse_point=i;
    end

end


plot(x)


i=1;
decode_result=[];
while (1)
    i=i+1;
    if x(i+1)-x(i)~=0
        startpoint=i+1;
        endpoint=startpoint+220;
         for j=startpoint:endpoint+1
            if x(j+1)-x(j)~=0
                decode_result=[decode_result,0];
%
                break
            end

            if j==endpoint+1
                decode_result=[decode_result,1];
%                 currentbit=1;
                break
            end
         end

         i=i+220;
    end

    if i >(length(x)-220)
        disp(num2str(decode_result))
        break
    end

end

end
calllib('USB3000','SetUSB3ClrAiTrigger',0)
calllib('USB3000','SetUSB3ClrAiFifo',0)
calllib('USB3000','USB3CloseDevice',0)
clear all;
unloadlibrary('USB3000')
