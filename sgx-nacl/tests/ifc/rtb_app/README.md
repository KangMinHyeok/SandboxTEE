# How to generate dataset?
Total 2 dataset - test data / training weight (learned by machine learning model)
- test data
Randomly generate test data.

Data format: "click(0 or 1) [??] features" "1 110 1235:1 552:1 674:1 ... "
Output: test_input.txt
1) enter "python3 genData.py"
2) enter the number of features ex) 10000
3) enter the number of profiles ex) 1

- training weight
Copy weights from original dataset: make-ipinyou-data/{1458, 3358, ..}/train.yzx.txt.lr.weight.

Data format: "feature probability" ex) 1251 0.91823
Output: weight_1458.txt, weight_3358.txt, weight_3386.txt, weight_3427.txt, weight_3476.txt
1) enter "python3 genWeight.py"
2) enter the number of features ex) 10000 (must be same with the input of genData.py)



# How to run the program?
(More than)4 entities: Client - SSP - Ad Exchange - [DSP1, DSP2, ...]
1) run DSPs: ./dsp [PORT] [# of features (must be same with test data)] [Bid algorithm (0 or 1)]
  ex) ./dsp 8888 10000 0
  You can run multiple DSPs but [PORT] must be increase by one
  ex) ./dsp 8888 10000 0 & ./dsp 8889 10000 1 & ...

2) run Ad Exchange: ./ad_exchange [SSP PORT] [DSP1 IP] [DSP2 IP] ... [DSP PORT]
  ex) ./ad_exchange 7777 147.46.215.133 147.46.215.133 8888
  You must pass all DSPs' IP, if you use multiple DSPs

3) run SSP: ./ssp [Client PORT] [Ad Exchange IP] [Ad Exchange PORT]
  ex) ./ssp 6666 147.46.215.133 7777

4) run Client(User): ./client [SSP IP] [SSP PORT]
  ex) ./cleint 147.46.215.133 6666

