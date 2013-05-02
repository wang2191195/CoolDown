#ifndef CLIENT_H
#define CLIENT_H

#include "error_code.h"
#include "local_sock_manager.h"
#include "net_task_manager.h"
#include "job_info.h"
#include <vector>
#include <string>
#include <boost/shared_ptr.hpp>
#include <Poco/Util/Application.h>
#include <Poco/Util/ServerApplication.h>
#include <Poco/Path.h>
#include <Poco/File.h>
#include <Poco/Types.h>
#include <Poco/ThreadPool.h>
#include <Poco/Timer.h>
#include <Poco/Condition.h>
#include <Poco/Mutex.h>
#include <google/protobuf/message.h>
using google::protobuf::Message;

using std::vector;
using std::string;
using boost::shared_ptr;
using Poco::Util::Application;
using Poco::Util::ServerApplication;
using Poco::Path;
using Poco::File;
using Poco::Int32;
using Poco::Int64;
using Poco::UInt64;
using Poco::TaskManager;
using Poco::ThreadPool;
using Poco::Timer;
using Poco::TimerCallback;
using Poco::Condition;
using Poco::FastMutex;

namespace Torrent{
    class Torrent;
}

namespace ClientProto{
    class ShakeHand;
}

namespace JobHistory{
    class JobHistoryInfo;
};

namespace CoolDown{
    namespace Client{

            class Job;

            typedef LocalSockManager::SockPtr SockPtr; 


            class CoolClient : public ServerApplication{
                public:

                    CoolClient();
                    enum {
                        TRACKER_PORT = 9977,
                    };
                    typedef vector<string> ClientIdCollection;
                    typedef vector<File> FileList;
                    typedef LocalSockManager::LocalSockManagerPtr LocalSockManagerPtr;
                    typedef int make_torrent_progress_callback_t;
                    typedef SharedPtr<Job> JobPtr;

                    //Application operations
                    void initialize(Application& self);
                    void uninitialize();
                    int main(const vector<string>& args);

                    //NetTaskManager& download_manager();
                    NetTaskManager& upload_manager();

                    //communicate with tracker
                    retcode_t login_tracker(const string& tracker_address, int port = TRACKER_PORT);
                    retcode_t logout_tracker(const string& tracker_address, int port = TRACKER_PORT);
                    retcode_t publish_resource_to_tracker(const string& tracker_address, const string& fileid);
                    retcode_t report_progress(const string& tracker_address, const string& fileid, int percentage);
                    retcode_t request_clients(const string& tracker_address, const string& fileid, int currentPercentage, 
                                          int needCount, const ClientIdCollection& clientids, FileOwnerInfoPtrList* pInfoList);

                    //communicate with client
                    retcode_t shake_hand(const ClientProto::ShakeHand& self, ClientProto::ShakeHand& peer);

                    //job control
                    retcode_t add_job(const Torrent::Torrent& torrent, const string& top_path, int* internal_handle);
                    
                    retcode_t SaveJobHistory(const string& filename);
                    retcode_t ReloadJobHistory(const string& filename);
                    retcode_t ReloadOneJob(const Torrent::Torrent& torrent, const JobHistory::JobHistoryInfo& history);
                    retcode_t AddNewJob(const string& torrent_path, const Torrent::Torrent& torrent, 
                            const FileIdentityInfoList& needs, const string& top_path, int* handle);
                    //retcode_t LoadDownload(const Torrent::Torrent& torrent, const Torrent::History& history, int* handle);
                    //retcode_t SerializeDownloadHistory();
                    //retcode_t ParseDownloadHistory();
                    retcode_t start_job(int handle);
                    retcode_t pause_download(int handle);
                    retcode_t resume_download(int handle);
                    JobPtr get_job(int handle);
                    JobPtr get_job(const string& fileid);

                    //collect Job runtime info
                    void onJobInfoCollectorWakeUp(Timer& timer);
                    Condition& GetInfoCollectorCond(){
                        return this->jobInfoCollectorTerminateCond_;
                    }



                    //torrent operations
                    retcode_t parse_torrent(const Path& torrent_file_path, Torrent::Torrent* pTorrent);
                    retcode_t make_torrent(const Path& path, const Path& torrent_file_path, 
                            Int32 chunk_size, Int32 type, const string& tracker_address);

                    //self identity
                    string clientid() const;


                    //utilities functions
                    string current_time() const;
                    static void format_speed(UInt64 speed, string* formatted_speed);

                    void set_make_torrent_progress_callback(make_torrent_progress_callback_t callback);


                private:

                    template<typename ReplyMessageType>
                    retcode_t handle_reply_message(LocalSockManager::SockPtr& sock, 
                    const Message& msg, int payload_type, SharedPtr<ReplyMessageType>* out);

                    void list_dir_recursive(const File& file, FileList* pList);

                    //no lock job ops
                    retcode_t pause_download_without_lock(int handle);
                    retcode_t resume_download_without_lock(int handle);

                    int LOCAL_PORT;

                    bool init_error_;
                    string clientid_;
                    string history_file_path_;
                    LocalSockManagerPtr sockManager_;

                    int job_index_;
                    typedef map<int,string> torrent_path_map_t;
                    torrent_path_map_t torrent_path_map_;

                    typedef map<int, JobPtr> JobMap;
                    //guard by mutex_;
                    JobMap jobs_;
                    FastMutex mutex_;

                    ThreadPool jobThreads_;

                    Condition jobInfoCollectorTerminateCond_;

                    //NetTaskManager downloadManager_;
                    NetTaskManager uploadManager_;

                    make_torrent_progress_callback_t make_torrent_progress_callback_;

            };
    }
}

#endif
