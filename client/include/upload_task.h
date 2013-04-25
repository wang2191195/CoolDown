#ifndef UPLOAD_TASK_H
#define UPLOAD_TASK_H

#include <string>
#include <Poco/Task.h>
#include <Poco/Net/StreamSocket.h>
#include <Poco/SharedPtr.h>
#include <Poco/File.h>
#include <Poco/Types.h>
#include <Poco/Format.h>

using std::string;
using Poco::Task;
using Poco::Net::StreamSocket;
using Poco::SharedPtr;
using Poco::File;
using Poco::UInt64;
using Poco::format;

namespace CoolDown{
    namespace Client{

        typedef SharedPtr<StreamSocket> SockPtr;
        typedef SharedPtr<File> FilePtr;

        class UploadTask : public Task{
            public:
                UploadTask(const FilePtr& file, UInt64 offset, int chunk_size, StreamSocket& sock);
                ~UploadTask();

                void runTask();
                UInt64 offset() const{
                    return this->offset_;
                }

                int chunk_size() const{
                    return this->chunk_size_;
                }

                string peer_address() const{
                    return sock_.peerAddress().toString();
                }

                string file_path() const{
                    return file_->path();
                }

                string DebugString() const{
                    return format("path = '%s', peer addr = '%s', offset = %Lu, chunk_size = %d", file_path(), peer_address(), offset(), chunk_size());
                }

            private:
                FilePtr file_;
                UInt64 offset_;
                int chunk_size_;
                StreamSocket& sock_;

        };
    }
}

#endif
