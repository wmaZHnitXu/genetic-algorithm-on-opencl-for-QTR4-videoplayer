#include "dmatrix.h"
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>

typedef struct {
    AVFormatContext *pFormatContext;
    AVCodecContext *pCodecContext;
    AVFrame *pFrame;
    AVFrame *pFrameRGB;
    struct SwsContext *sws_ctx;
    int videoStreamIndex;
    int width;
    int height;
    uint8_t *buffer;
    AVPacket *packet;
    int frameFinished;
} FFmpegState;

int initFFmpegState(FFmpegState *state, const char* filename);
DMatrix* readNextFrame(FFmpegState *state);
void freeFFmpegState(FFmpegState *state);
