#include <sndfile.h>

#include "idatanode.h"
#include "buffer.h"
#include "converter.h"

namespace Loom
{

    class FileNode : public IDataNode
    {
    public:
        FileNode(const char* filePath)
            : filePath(filePath)
            , file(nullptr)
        {
            SF_INFO sfinfo{};
            file = sf_open(filePath, SFM_READ, &sfinfo);

            if (file)
            {
                format.channelCount = sfinfo.channels;
                format.frequency = static_cast<unsigned>(sfinfo.samplerate);
                frameCount = static_cast<unsigned>(sfinfo.frames);
            }
        }

        ~FileNode() override
        {
            if (file)
                sf_close(file);

            if (fileBuffer)
                delete[] fileBuffer;
        }

        Error FileNode::Load(Format requestedFormat)
        {
            if (!file)
                return Failed;

            // Allocate buffer for the whole file in its original format
            Buffer originalBuffer(frameCount, format);
            unsigned framesRead = sf_readf_float(file, originalBuffer.data.data(), frameCount);
            if (framesRead != frameCount)
                return Failed;

            // Check if the file format matches the requested format
            if (format == requestedFormat)
            {
                fileBuffer = new Buffer(frameCount, requestedFormat);
                fileBuffer->data = std::move(originalBuffer.data);
                return OK;
            }

            // Handle format conversion if needed
            fileBuffer = new Buffer(frameCount, requestedFormat);

            if (format.frequency != requestedFormat.frequency)
            {
                Error resampleError = Converter::Resample(&originalBuffer, fileBuffer, frameCount);
                if (resampleError != OK)
                    return resampleError;
            }

            if (format.channelCount != requestedFormat.channelCount)
            {
                if (requestedFormat.channelCount == 2 && format.channelCount == 1)
                    Converter::MonoToStereo(&originalBuffer, fileBuffer, frameCount);
                else if (requestedFormat.channelCount == 1 && format.channelCount == 2)
                    Converter::StereoToMono(&originalBuffer, fileBuffer, frameCount);
                else
                    return FormatMismatch;
            }

            return OK;
        }

        Error FileNode::Read(Buffer* buffer, unsigned bufferFrameOffset, unsigned framePosition, unsigned frameCount, unsigned& framesRead)
        {
            if (!fileBuffer)
                return Failed;

            // Ensure the output buffer matches the format of the loaded buffer
            if (buffer->format != fileBuffer->format)
                return FormatMismatch;

            // Check bounds and calculate the number of frames to read
            unsigned remainingFrames = fileBuffer->FrameCapacity() - framePosition;
            framesRead = std::min(frameCount, remainingFrames);

            // Copy data from the loaded buffer to the output buffer
            float* bufferPointer = nullptr;
            float* fileBufferPointer = nullptr;
            Error error = buffer->FrameOffset(bufferFrameOffset, bufferPointer);
            if (error)
                return error;
            error = fileBuffer->FrameOffset(framePosition, fileBufferPointer);
            if (error)
                return error;

            std::memcpy(bufferPointer,
                        fileBufferPointer,
                        framesRead * fileBuffer->format.channelCount * sizeof(float));

            return OK;
        }

        Error GetFrameCount(unsigned& frameCount) override
        {
            frameCount = frameCount;
            return OK;
        }

        Error GetFormat(Format& format) override
        {
            if (!file)
                return Failed;

            format = format;
            return OK;
        }

        Error GetDuration(float& seconds) override
        {
            if (!file)
                return Failed;

            seconds = static_cast<float>(frameCount) / static_cast<float>(format.frequency);
            return OK;
        }

    private:
        SF_INFO sfinfo;
        SNDFILE* file;

        const char* filePath;
        unsigned frameCount;
        Format format;
        Buffer* fileBuffer;
    };

}  // namespace Loom
