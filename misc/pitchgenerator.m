f = 50:800;
fs = 44100;
t = (0:(2*fs))/fs;
nFreq = length(f);

for iFreq = 1:nFreq
    f(iFreq)
    x = zeros(size(t));
    for iAdd = 1:100
        x = x + cos(2*pi*f(iFreq)*t*iAdd)*0.6^(iAdd-1);
        if f(iFreq)*iAdd > fs/2
            break
        end
    end
    x = x / max(abs(x));
    wavwrite(x, fs, sprintf('/Users/matthiasm/data/pyin/simplewavs/%iHz.wav', f(iFreq)));
end