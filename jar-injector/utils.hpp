#include <jni.h>
#include "j_classloader.hpp"

namespace utils {
	JavaVM* get_jvm_instance();
	j_classloader* get_classloader_by_thread_name(const char* thread_name);
	jobject file_path_to_java_file(const char* path);
	jobject java_file_to_java_uri(jobject file);
	jobject java_uri_to_java_url(jobject uri);

	void add_file_path_to_url_classloader(j_classloader* classloader, const char* file_path);
}