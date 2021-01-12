#include <stdio.h>
#include <stdlib.h>
#include <sndfile.h>

const int BLOCK_SIZE = 8192;
const int CHANNEL_BLOCK_SIZE = 8256;
const int FIRST_BLOCK_DIVISION_SIZE = 128;
const int SECOND_BLOCK_DIVISION_SIZE = 64;
const int SECOND_CHANNEL_BLOCK_DIVISION_SIZE = 68;
const int THIRD_BLOCK_DIVISION_SIZE = 16;
const int FOURTH_BLOCK_DIVISION_SIZE = 8;
const int FIFTH_BLOCK_DIVISION_SIZE = 4;

SNDFILE * sf;
SF_INFO info;
int num_of_channels;
int num, file_cnt, num_items;
double * buf, * bufs, * first_channel, * first_channel_scrambled1, * first_channel_scrambled, * first_channel_descrambled1, * first_channel_descrambled, * second_channel_scrambled, * second_channel_descrambled, * second_channel_descrambled1, * second_channel_scrambled1, * second_channel;
int items_in_channels;
int frames, samplerate, channels;
FILE * out, * out1, * out2, * out3, * out4, * out5, * out6;
int remainder_;
int temp;
int number_of_frames, counter;
// Load the audiofile
void load(char ** argv);
// Print information about samplerate,number of frames,remainder_
void print_info();
// Scrabling algorithm
void scrambler();
// Descrambling algorithm
void descrambler();
// Write to files
void write_to_files();

int main(int argc, char ** argv) {

    load(argv);
    print_info();
    scrambler();
    descrambler();
    write_to_files();

    return 0;
}
void load(char ** argv) {
    /*Open the WAV file. */
    info.format = 0;
    char * audiofile;
    audiofile = argv[1];
    sf = sf_open(audiofile, SFM_READ, & info);
    if (sf == NULL) {
        printf("Failed to open the file.\n");
        exit(-1);
    }
}
void print_info() {
    // Print some of the info, and figure out how much data to read
    frames = info.frames;
    samplerate = info.samplerate;
    channels = info.channels;
    printf("Number of frames = %d\n", frames);
    printf("Samplerate = %d\n", samplerate);
    printf("Number of channels = %d\n", channels);
    num_items = frames * channels;
    items_in_channels = num_items / 2;
    printf("Number of items = %d\n", num_items);

    // Allocate space for the data to be read, then read it
    buf = (double * ) malloc(num_items * sizeof(double));
    bufs = (double * ) malloc(num_items * sizeof(double));
    first_channel = (double * ) malloc((items_in_channels) * sizeof(double));
    second_channel = (double * ) malloc((items_in_channels) * sizeof(double));
    first_channel_scrambled = (double * ) malloc((items_in_channels) * sizeof(double));
    first_channel_scrambled1 = (double * ) malloc((items_in_channels) * sizeof(double));
    second_channel_scrambled = (double * ) malloc((items_in_channels) * sizeof(double));
    second_channel_scrambled1 = (double * ) malloc((items_in_channels) * sizeof(double));
    first_channel_descrambled = (double * ) malloc((items_in_channels) * sizeof(double));
    first_channel_descrambled1 = (double * ) malloc((items_in_channels) * sizeof(double));
    second_channel_descrambled = (double * ) malloc((items_in_channels) * sizeof(double));
    second_channel_descrambled1 = (double * ) malloc((items_in_channels) * sizeof(double));
    num = sf_read_double(sf, buf, num_items);

    sf_close(sf);

    printf("Read %d items\n", num);
    remainder_ = items_in_channels % BLOCK_SIZE;
    printf("Remainder = %d", remainder_);
    number_of_frames = (items_in_channels - remainder_) / BLOCK_SIZE;
    out = fopen("first_channel.out", "w");
    out1 = fopen("second_channel.out", "w");
    out2 = fopen("first_channel_scrambled.out", "w");
    out3 = fopen("second_channel_scrambled.out", "w");
    out5 = fopen("first_channel_descrambled.out", "w");
    out6 = fopen("second_channel_descrambled.out", "w");
    out4 = fopen("filedata.out", "w");

}
void scrambler() {
    for (int i = 2; i < num + 2; i += 2) {
        int k = i / 2;
        first_channel[k] = buf[i];
    }
    for (int i = 3; i < num + 3; i += 2) {
        int d = (i - 1) / 2;
        second_channel[d] = buf[i];
    }

    file_cnt = 1;
    counter = 1;

    for (int i = 1; i <= number_of_frames; i++) {
        int n = 4;

        for (int j = counter; j <= counter + BLOCK_SIZE - 1; j++) {
            int k = 8192 - n + j;

            if (j % 4 == 0) {
                n = n + 8;
            }
            first_channel_scrambled[k] = first_channel[j];
            second_channel_scrambled[k] = second_channel[j];

        }

        counter = counter + BLOCK_SIZE;
    }

    for (int j = counter; j <= counter + remainder_ - 1; j++) {
        first_channel_scrambled[j] = first_channel[j];
        second_channel_scrambled[j] = second_channel[j];
    }
    for (int i = 1; i < items_in_channels; i++) {
        fprintf(out2, "%lf ", first_channel_scrambled[i]);
        fprintf(out2, "\n");
        fprintf(out3, "%lf ", second_channel_scrambled[i]);
        fprintf(out3, "\n");
    }
    for (int i = 0; i < items_in_channels; i += 1) {
        temp = (i * 2) + 1;
        bufs[i * 2] = first_channel_scrambled[i];
        bufs[temp] = second_channel_scrambled[i];
    }
    for (int i = 0; i < num; i++) {
        fprintf(out4, "%lf ", bufs[i]);
        fprintf(out4, "\n");
    }
}
void descrambler() {
    counter = 1;
    for (int i = 1; i <= number_of_frames; i++) {
        int n = 4;

        for (int j = counter; j <= counter + BLOCK_SIZE - 1; j++) {
            int k = 8192 - n + j;

            if (j % 4 == 0) {
                n = n + 8;
            }
            first_channel_descrambled[k] = first_channel_scrambled[j];
            second_channel_descrambled[k] = second_channel_scrambled[j];

        }

        counter = counter + BLOCK_SIZE;
    }
    for (int j = counter; j <= counter + remainder_ - 1; j++) {
        first_channel_descrambled[j] = first_channel[j];
        second_channel_descrambled[j] = second_channel[j];
    }
}
void write_to_files() {
    for (file_cnt; file_cnt < items_in_channels; file_cnt++) {
        fprintf(out5, "%lf ", first_channel_descrambled[file_cnt]);
        fprintf(out5, "\n");
        fprintf(out6, "%lf ", second_channel_descrambled[file_cnt]);
        fprintf(out6, "\n");
        fprintf(out, "%lf ", first_channel[file_cnt]);
        fprintf(out, "\n");
        fprintf(out1, "%lf ", second_channel[file_cnt]);
        fprintf(out1, "\n");
    }
    fclose(out5);
    fclose(out6);
    fclose(out1);
    fclose(out);
}
