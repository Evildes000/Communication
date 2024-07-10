import argparse
import os
import matplotlib.pyplot as plt
import numpy as np


#plot graphics
def plot_delay(seq: np.ndarray, delay: np.ndarray):
    plt.plot(delay,seq,'o')
    #plt.xlim(0,400000)
    plt.show()



#this function process data in the given file so that they can be used to plot graph later
def file_parse(filename: str, delimiter: str = ":"):

    #filename = os.path.join(filename,'.txt')
    #open file in reading mode
    file_object = open(filename, 'r')
    lines = file_object.readlines()
    length = len(lines)
    print("number of lines is: ", length)

    simu_time = np.zeros(shape = (length,1))
    seq_pkt = np.zeros(shape = (length,1))
    delay_per_pkt = np.zeros(shape = (length,1))
    
    #my_dict is used for store sequence number of packets(key) and delay for the packet(value)
    my_dict = {}

    iter = 0
    for line in lines:
        print(line)
        line_list = line.split(delimiter)
        simu_time[iter] = int(line_list[0])
        #key = int(line_list[1])
        #value = int(line_list[2])
        #my_dict[key] = value
        seq_pkt[iter] = int(line_list[1])
        delay_per_pkt[iter] = int(line_list[2])
        iter += 1
    #np.sort(delay_per_pkt)
    #pack seq_pkt as key and delay_per_pkt as value in a dict

    print(my_dict)
    plot_delay(seq_pkt,delay_per_pkt)



if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument("--simu_result", type = str, help = "txt result file from ns3 simulation")
    parser.add_argument("--plot_name", type = str, help = "Name of the output graph file", default = "plot_output")
    args = parser.parse_args()

    if not args.simu_result:
        raise ValueError("Please specify a result file from ns3 simulation ")
    
    if not os.path.exists(args.simu_result):
        raise ValueError("Can't find the file")
    else:
        file_parse(args.simu_result)

