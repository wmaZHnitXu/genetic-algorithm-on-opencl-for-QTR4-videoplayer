#include "video_import.h"
#include <stdio.h>
#include <stdlib.h>

int initFFmpegState(FFmpegState *state, const char* filename) {
    printf("Initializing FFmpeg state...\n");

    avformat_network_init();
    state->pFormatContext = NULL;
    state->pCodecContext = NULL;
    state->pFrame = NULL;
    state->pFrameRGB = NULL;
    state->sws_ctx = NULL;
    state->buffer = NULL;
    state->packet = NULL;

    printf("Opening input file: %s\n", filename);
    if (avformat_open_input(&state->pFormatContext, filename, NULL, NULL) != 0) {
        fprintf(stderr, "Ошибка: не удалось открыть файл.\n");
        return -1;
    }

    printf("Finding stream info...\n");
    if (avformat_find_stream_info(state->pFormatContext, NULL) < 0) {
        fprintf(stderr, "Ошибка: не удалось получить информацию о потоке.\n");
        return -1;
    }

    printf("Finding video stream...\n");
    state->videoStreamIndex = -1;
    for (int i = 0; i < state->pFormatContext->nb_streams; i++) {
        if (state->pFormatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            state->videoStreamIndex = i;
            break;
        }
    }
    if (state->videoStreamIndex == -1) {
        fprintf(stderr, "Ошибка: не удалось найти видеопоток.\n");
        return -1;
    }

    AVCodecParameters *pCodecParameters = state->pFormatContext->streams[state->videoStreamIndex]->codecpar;
    AVCodec *pCodec = avcodec_find_decoder(pCodecParameters->codec_id);
    if (pCodec == NULL) {
        fprintf(stderr, "Ошибка: не удалось найти декодер.\n");
        return -1;
    }

    printf("Allocating codec context...\n");
    state->pCodecContext = avcodec_alloc_context3(pCodec);
    if (!state->pCodecContext) {
        fprintf(stderr, "Ошибка: не удалось выделить память для контекста кодека.\n");
        return -1;
    }

    printf("Copying codec parameters to codec context...\n");
    if (avcodec_parameters_to_context(state->pCodecContext, pCodecParameters) < 0) {
        fprintf(stderr, "Ошибка: не удалось скопировать параметры кодека.\n");
        return -1;
    }

    printf("Opening codec...\n");
    if (avcodec_open2(state->pCodecContext, pCodec, NULL) < 0) {
        fprintf(stderr, "Ошибка: не удалось открыть кодек.\n");
        return -1;
    }

    printf("Allocating frames...\n");
    state->pFrame = av_frame_alloc();
    if (!state->pFrame) {
        fprintf(stderr, "Ошибка: не удалось выделить память для кадра.\n");
        return -1;
    }

    state->pFrameRGB = av_frame_alloc();
    if (!state->pFrameRGB) {
        fprintf(stderr, "Ошибка: не удалось выделить память для RGB кадра.\n");
        return -1;
    }

    state->width = state->pCodecContext->width;
    state->height = state->pCodecContext->height;
    int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGB24, state->width, state->height, 32);
    printf("Allocating buffer (size: %d)...\n", numBytes);
    state->buffer = (uint8_t *)av_malloc(numBytes * sizeof(uint8_t));
    if (!state->buffer) {
        fprintf(stderr, "Ошибка: не удалось выделить память для буфера.\n");
        return -1;
    }
    av_image_fill_arrays(state->pFrameRGB->data, state->pFrameRGB->linesize, state->buffer, AV_PIX_FMT_RGB24, state->width, state->height, 32);

    printf("Creating SWS context...\n");
    state->sws_ctx = sws_getContext(
        state->width,
        state->height,
        state->pCodecContext->pix_fmt,
        state->width,
        state->height,
        AV_PIX_FMT_RGB24,
        SWS_BILINEAR,
        NULL,
        NULL,
        NULL
    );
    if (!state->sws_ctx) {
        fprintf(stderr, "Ошибка: не удалось создать контекст преобразования.\n");
        return -1;
    }

    printf("Allocating packet...\n");
    state->packet = av_packet_alloc();
    if (!state->packet) {
        fprintf(stderr, "Ошибка: не удалось выделить память для пакета.\n");
        return -1;
    }

    printf("FFmpeg state initialized successfully.\n");
    return 0;
}

DMatrix* readNextFrame(FFmpegState *state) {
    while (av_read_frame(state->pFormatContext, state->packet) >= 0) {
        if (state->packet->stream_index == state->videoStreamIndex) {
            if (avcodec_send_packet(state->pCodecContext, state->packet) != 0) {
                av_packet_unref(state->packet);
                continue;
            }

            if (avcodec_receive_frame(state->pCodecContext, state->pFrame) == 0) {
                sws_scale(
                    state->sws_ctx,
                    (uint8_t const *const *)state->pFrame->data,
                    state->pFrame->linesize,
                    0,
                    state->height,
                    state->pFrameRGB->data,
                    state->pFrameRGB->linesize
                );

                DMatrix* matrix = allocDMatrix(state->width, state->height);

                for (int y = 0; y < state->height; y++) {
                    for (int x = 0; x < state->width; x++) {
                        int r = state->pFrameRGB->data[0][y * state->pFrameRGB->linesize[0] + x * 3];
                        int g = state->pFrameRGB->data[0][y * state->pFrameRGB->linesize[0] + x * 3 + 1];
                        int b = state->pFrameRGB->data[0][y * state->pFrameRGB->linesize[0] + x * 3 + 2];
                        int color = (0xFF << 24) | (r << 16) | (g << 8) | b;  // Формат 0xAARRGGBB с прозрачностью 0xFF
                        setDMatrixElem(matrix, y, x, color);
                    }
                }

                av_packet_unref(state->packet);
                return matrix;
            }
        }
        av_packet_unref(state->packet);
    }

    return NULL; // Нет больше кадров
}

void freeFFmpegState(FFmpegState *state) {
    if (state->buffer) av_free(state->buffer);
    if (state->pFrameRGB) av_frame_free(&state->pFrameRGB);
    if (state->pFrame) av_frame_free(&state->pFrame);
    if (state->pCodecContext) avcodec_free_context(&state->pCodecContext);
    if (state->pFormatContext) avformat_close_input(&state->pFormatContext);
    if (state->packet) av_packet_free(&state->packet);
    avformat_network_deinit();
}
