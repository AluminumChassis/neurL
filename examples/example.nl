-- data
weights < 3, 0.0;
inputs < 0.0, 1.0, 1.0, 1.0, 0.0, 1.0;
outputs < 0.0, 1.0;
-- math
neuron < sigmoid{+x};
-- runtime
setupWeights < ;
run < 0;
run < 3;
train < 100000;
run < 0;
run < 3;
-- end