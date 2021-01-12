#include "tb_vp.hpp"
#include "vp_addr.hpp"
#include <string>
#include <tlm_utils/tlm_quantumkeeper.h>
#include <tlm_utils/tlm_quantumkeeper.h>
#include <sstream>
#include <iostream>
#include <sndfile.h>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <deque>
#include <vector>
#include <cmath>

using namespace sc_core;
using namespace sc_dt;
using namespace std;
using namespace tlm;

SC_HAS_PROCESS(tb_vp);

tb_vp::tb_vp(sc_module_name name, char **argv):
    sc_module(name)
    {

        audiofile = argv;

        SC_THREAD(test);
    }

void tb_vp::test()
{

    SNDFILE * sf;
    SF_INFO info;
    int num, file_cnt, num_items;
    int items_in_channels;
    int frames, samplerate, channels;
    FILE *out, *out1, *out2, *out3, *out5, *out6;
    int remainder;
    int number_of_frames;

    /*Open the WAV file. */
    info.format = 0;

    sf = sf_open(*audiofile, SFM_READ, &info);
    if (sf == NULL)
    {
        printf("Failed to open the file.\n");
        exit(-1);
    }
    frames = info.frames;
    samplerate = info.samplerate;
    channels = info.channels;
    cout << "Number of frames = " << frames << endl;
    cout << "Samplerate = " << samplerate << endl;
    cout << "Channels = " << channels << endl;
    num_items = frames * channels;
    items_in_channels = num_items / 2;
    cout << "Number of items = " << num_items << endl;
    double *buff = new double[num_items];
    double *first_channel = new double[items_in_channels];
    double *first_channel_scrambled = new double[items_in_channels];

    double *first_channel_descrambled = new double[items_in_channels];
    double *second_channel_descrambled = new double[items_in_channels];
    double *second_channel_scrambled = new double[items_in_channels];

    double *second_channel = new double[items_in_channels];

    num = sf_read_double(sf, buff, num_items);
    sf_close(sf);
    cout << "Read " << num << " items" << endl;
    remainder = items_in_channels % BLOCK_SIZE;
    cout << "Remainder = " << remainder << endl;
    file_cnt = 1;
    number_of_frames = (items_in_channels - remainder) / BLOCK_SIZE;
    out = fopen("first_channel.out", "w");
    out1 = fopen("second_channel.out", "w");
    out2 = fopen("first_channel_scrambled.out", "w");
    out3 = fopen("second_channel_scrambled.out", "w");
    out5 = fopen("first_channel_descrambled.out", "w");
    out6 = fopen("second_channel_descrambled.out", "w");

    for (int i = 2; i < num; i += 2)
    {
        int k = i / 2;
        first_channel[k] = buff[i];
    }
    for (int i = 3; i < num + 1; i += 2)
    {
        int d = (i - 1) / 2;
        second_channel[d] = buff[i];
    }
    sc_time loct;
    tlm_generic_payload pl;
    tlm_utils::tlm_quantumkeeper qk;
    qk.reset();

    int counter = 1;
    int counter_read = 1;
    tlm_command cmd;
    unsigned int addr = 1;
    sc_time offset = SC_ZERO_TIME;
    string msg;
   	//Sending scrambler data and receiving scrambled read data to end of file
    for (int i = 1; i <= number_of_frames; i++)
    {
        addr = VP_ADDR_SCRAMBLER;
        for (int j = 0; j != BLOCK_SIZE; ++j)
        {

            cmd = TLM_WRITE_COMMAND;
            sc_fixed<TOTAL_NUM_BIT_WIDTH, 2> numm = first_channel[counter];
            counter++;

            unsigned int nummm = numm << NUM_BIT_WIDTH;
            sc_uint<TOTAL_NUM_BIT_WIDTH> numms = nummm;

            pl.set_command(cmd);
            pl.set_address(addr);
            pl.set_data_ptr((unsigned char *) &numms);
            pl.set_data_length(3);
            pl.set_response_status(TLM_INCOMPLETE_RESPONSE);
            #ifdef QUANTUM
                qk.inc(sc_time(4, SC_NS));
                offset = qk.get_local_time();
            #else
                offset += sc_time(4, SC_NS);
            #endif

            isoc->b_transport(pl, offset);
        }
        for (int j = 0; j != BLOCK_SIZE; j++)
        {
            cmd = TLM_READ_COMMAND;
            sc_uint<TOTAL_NUM_BIT_WIDTH> tmp;
            pl.set_command(cmd);
            pl.set_address(addr);
            pl.set_data_ptr((unsigned char *) &tmp);
            pl.set_data_length(3);
            pl.set_response_status(TLM_INCOMPLETE_RESPONSE);
            #ifdef QUANTUM
                qk.inc(sc_time(4, SC_NS));
                offset = qk.get_local_time();
            #else
                offset += sc_time(4, SC_NS);
            #endif

            isoc->b_transport(pl, offset);
            unsigned int nummss = tmp;
            sc_fixed<TOTAL_NUM_BIT_WIDTH, 2> num1 = double(nummss) / (1 << NUM_BIT_WIDTH);
            double end = num1;
            first_channel_scrambled[counter_read] = num1;
            counter_read++;
            fprintf(out2, "%5.6f", end);
            fprintf(out2, "\n");
        }
    }
    counter = 1;
    counter_read = 1;
    for (int i = 1; i <= number_of_frames; i++)
    {
        addr = VP_ADDR_SCRAMBLER;
        for (int j = 0; j != BLOCK_SIZE; ++j)
        {
            cmd = TLM_WRITE_COMMAND;
            sc_fixed<TOTAL_NUM_BIT_WIDTH, 2> numm = second_channel[counter];
            counter++;

            unsigned int nummm = numm << NUM_BIT_WIDTH;
            sc_uint<TOTAL_NUM_BIT_WIDTH> numms = nummm;
            pl.set_command(cmd);
            pl.set_address(addr);
            pl.set_data_ptr((unsigned char *) &numms);
            pl.set_data_length(3);
            pl.set_response_status(TLM_INCOMPLETE_RESPONSE);
            #ifdef QUANTUM
                qk.inc(sc_time(4, SC_NS));
                offset = qk.get_local_time();
            #else
                offset += sc_time(4, SC_NS);
            #endif

            isoc->b_transport(pl, offset);
        }
        for (int j = 0; j != BLOCK_SIZE; j++)
        {
            cmd = TLM_READ_COMMAND;
            sc_uint<TOTAL_NUM_BIT_WIDTH> tmp;
            pl.set_command(cmd);
            pl.set_address(addr);
            pl.set_data_ptr((unsigned char *) &tmp);
            pl.set_data_length(3);
            pl.set_response_status(TLM_INCOMPLETE_RESPONSE);
            #ifdef QUANTUM
                qk.inc(sc_time(4, SC_NS));
                offset = qk.get_local_time();
            #else
                offset += sc_time(4, SC_NS);
            #endif

            isoc->b_transport(pl, offset);

            unsigned int nummss = tmp;
            sc_fixed<TOTAL_NUM_BIT_WIDTH, 2> num1 = double(nummss) / (1 << NUM_BIT_WIDTH);
            double end = num1;
            second_channel_scrambled[counter_read] = num1;
            counter_read++;
           	//Writting to a file
            fprintf(out3, "%5.6f", end);
            fprintf(out3, "\n");
        }
    }
    for (int remainder_counter = number_of_frames *BLOCK_SIZE + 1; remainder_counter <= items_in_channels; remainder_counter++)
    {
        first_channel_scrambled[remainder_counter] = first_channel[remainder_counter];
        second_channel_scrambled[remainder_counter] = second_channel[remainder_counter];
    }
    counter = 1;
    counter_read = 1;
    for (int i = 1; i <= number_of_frames; i++)
    {
        addr = VP_ADDR_SCRAMBLER;
        for (int j = 0; j != BLOCK_SIZE; ++j)
        {
            cmd = TLM_WRITE_COMMAND;
            sc_fixed<TOTAL_NUM_BIT_WIDTH, 2> numm = first_channel_scrambled[counter];
            counter++;

            unsigned int nummm = numm << NUM_BIT_WIDTH;
            sc_uint<TOTAL_NUM_BIT_WIDTH> numms = nummm;

            pl.set_command(cmd);
            pl.set_address(addr);
            pl.set_data_ptr((unsigned char *) &numms);
            pl.set_data_length(3);
            pl.set_response_status(TLM_INCOMPLETE_RESPONSE);
            #ifdef QUANTUM
                qk.inc(sc_time(4, SC_NS));
                offset = qk.get_local_time();
            #else
                offset += sc_time(4, SC_NS);
            #endif

            isoc->b_transport(pl, offset);
        }
        for (int j = 0; j != BLOCK_SIZE; j++)
        {
            cmd = TLM_READ_COMMAND;
            sc_uint<TOTAL_NUM_BIT_WIDTH> tmp;
            pl.set_command(cmd);
            pl.set_address(addr);
            pl.set_data_ptr((unsigned char *) &tmp);
            pl.set_data_length(3);
            pl.set_response_status(TLM_INCOMPLETE_RESPONSE);
            #ifdef QUANTUM
                qk.inc(sc_time(4, SC_NS));
                offset = qk.get_local_time();
            #else
                offset += sc_time(4, SC_NS);
            #endif

            isoc->b_transport(pl, offset);
            unsigned int nummss = tmp;
            sc_fixed<TOTAL_NUM_BIT_WIDTH, 2> num1 = double(nummss) / (1 << NUM_BIT_WIDTH);
            first_channel_descrambled[counter_read] = num1;

            counter_read++;
        }
    }
    counter = 1;
    counter_read = 1;
    for (int i = 1; i <= number_of_frames; i++)
    {
        addr = VP_ADDR_SCRAMBLER;
        for (int j = 0; j != BLOCK_SIZE; ++j)
        {

            cmd = TLM_WRITE_COMMAND;
            sc_fixed<TOTAL_NUM_BIT_WIDTH, 2> numm = second_channel_scrambled[counter];
            counter++;

            unsigned int nummm = numm << NUM_BIT_WIDTH;
            sc_uint<TOTAL_NUM_BIT_WIDTH> numms = nummm;

            pl.set_command(cmd);
            pl.set_address(addr);
            pl.set_data_ptr((unsigned char *) &numms);
            pl.set_data_length(3);
            pl.set_response_status(TLM_INCOMPLETE_RESPONSE);
            #ifdef QUANTUM
                qk.inc(sc_time(4, SC_NS));
                offset = qk.get_local_time();
            #else
                offset += sc_time(4, SC_NS);
            #endif

            isoc->b_transport(pl, offset);
        }
        for (int j = 0; j != BLOCK_SIZE; j++)
        {
            cmd = TLM_READ_COMMAND;
            sc_uint<TOTAL_NUM_BIT_WIDTH> tmp;
            pl.set_command(cmd);
            pl.set_address(addr);
            pl.set_data_ptr((unsigned char *) &tmp);
            pl.set_data_length(3);
            pl.set_response_status(TLM_INCOMPLETE_RESPONSE);
            #ifdef QUANTUM
                qk.inc(sc_time(4, SC_NS));
                offset = qk.get_local_time();
            #else
                offset += sc_time(4, SC_NS);
            #endif

            isoc->b_transport(pl, offset);
            unsigned int nummss = tmp;
            sc_fixed<TOTAL_NUM_BIT_WIDTH, 2> num1 = double(nummss) / (1 << NUM_BIT_WIDTH);
            second_channel_descrambled[counter_read] = num1;
            counter_read++;
        }
    }
    for (int remainder_counter = number_of_frames *BLOCK_SIZE + 1; remainder_counter <= items_in_channels; remainder_counter++)
    {
        first_channel_descrambled[remainder_counter] = first_channel_scrambled[remainder_counter];
        second_channel_descrambled[remainder_counter] = second_channel_scrambled[remainder_counter];
    }
    for (file_cnt; file_cnt <= number_of_frames * BLOCK_SIZE; file_cnt++)
    {
        fprintf(out6, "%5.6f", second_channel_descrambled[file_cnt]);
        fprintf(out6, "\n");
        fprintf(out5, "%5.6f", first_channel_descrambled[file_cnt]);
        fprintf(out5, "\n");
        fprintf(out, "%lf ", first_channel[file_cnt]);
        fprintf(out, "\n");
        fprintf(out1, "%lf ", second_channel[file_cnt]);
        fprintf(out1, "\n");
    }
    for (int cnt = number_of_frames * BLOCK_SIZE; cnt != number_of_frames *BLOCK_SIZE + remainder; cnt++)
    {
        fprintf(out, "%5.6f", first_channel[cnt]);
        fprintf(out, "\n");
        fprintf(out1, "%5.6f", second_channel[cnt]);
        fprintf(out1, "\n");
        fprintf(out5, "%5.6f", first_channel[cnt]);
        fprintf(out5, "\n");
        fprintf(out6, "%5.6f", second_channel[cnt]);
        fprintf(out6, "\n");
        fprintf(out2, "%5.6f", first_channel[cnt]);
        fprintf(out2, "\n");
        fprintf(out3, "%5.6f", second_channel[cnt]);
        fprintf(out3, "\n");
    }
   	//Monitoring
    int simulation_passed = 0;
    for (int jjj = 1; jjj <= items_in_channels; jjj++)
    {
        if (first_channel[jjj] != first_channel_descrambled[jjj])
        {
            simulation_passed++;
            cout << "Simulation failed on " << jjj << " member of a first_channel_scrambled" << endl;
            cout << "first_channel = " << first_channel[jjj] << " and first_channel_descrambled = " << first_channel_descrambled[jjj] << endl;

            jjj = items_in_channels;
        }
        if (second_channel[jjj] != second_channel_descrambled[jjj])
        {
            simulation_passed++;
            cout << "Simulation failed on " << jjj << " member of a second_channel_scrambled" << endl;
            cout << "second_channel = " << second_channel[jjj] << " and second_channel_descrambled = " << second_channel_descrambled[jjj] << endl;
            jjj = items_in_channels;
        }
    }
    if (simulation_passed == 0)
    {
        cout << "Simulation_passed" << endl;
    }
   	//Finished sending and receiving scrambled data
    cout << "Scrambling results are written to files. ";
    sc_stop();
}
