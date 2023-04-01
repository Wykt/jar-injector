#include "utils.hpp"
#include <Windows.h>
#include "injector.hpp"

typedef long(__stdcall* _JNI_GetCreatedJavaVMs)(JavaVM**, long, long*);

JavaVM* utils::get_jvm_instance()
{
	JavaVM* jvm;

	HMODULE jvm_handle = GetModuleHandleA("jvm.dll");
	_JNI_GetCreatedJavaVMs JNI_GetCreatedJavaVMs = reinterpret_cast<_JNI_GetCreatedJavaVMs>(GetProcAddress(reinterpret_cast<HMODULE>(jvm_handle), "JNI_GetCreatedJavaVMs"));
	JNI_GetCreatedJavaVMs(&jvm, 1, nullptr);

	return jvm;
}

j_classloader* utils::get_classloader_by_thread_name(const char* thread_name)
{
    jint threadsCount;
    jthread* threads;

    client->jvmti->GetAllThreads(&threadsCount, &threads);

    for (int i = 0; i < threadsCount; ++i)
    {
        jthread thread = threads[i];
        jvmtiThreadInfo threadInfo;
        client->jvmti->GetThreadInfo(thread, &threadInfo);

        if (strcmp(thread_name, threadInfo.name) == 0) {
            return new j_classloader(threadInfo.context_class_loader);
        }
    }

    return NULL;
}

jobject utils::file_path_to_java_file(const char* path)
{
    jclass c_file = client->env->FindClass("java/io/File");
    jmethodID m_file_init = client->env->GetMethodID(c_file, "<init>", "(Ljava/lang/String;)V");
    jstring j_path = client->env->NewStringUTF(path);

    jobject o_file = client->env->NewObject(c_file, m_file_init, j_path);

    client->env->DeleteLocalRef(c_file);
    client->env->DeleteLocalRef(j_path);

    return o_file;
}

jobject utils::java_file_to_java_uri(jobject file)
{
    jclass c_file = client->env->FindClass("java/io/File");
    jmethodID m_file_toURI = client->env->GetMethodID(c_file, "toURI", "()Ljava/net/URI;");

    jobject o_uri = client->env->CallObjectMethod(file, m_file_toURI);

    client->env->DeleteLocalRef(c_file);

    return o_uri;
}

jobject utils::java_uri_to_java_url(jobject uri)
{
    jclass c_uri = client->env->FindClass("java/net/URI");
    jmethodID m_uri_toURL = client->env->GetMethodID(c_uri, "toURL", "()Ljava/net/URL;");

    jobject o_url = client->env->CallObjectMethod(uri, m_uri_toURL);

    client->env->DeleteLocalRef(c_uri);

    return o_url;
}

void utils::add_file_path_to_url_classloader(j_classloader* classloader, const char* file_path)
{
    jobject file = file_path_to_java_file(file_path);
    jobject uri = java_file_to_java_uri(file);
    jobject url = java_uri_to_java_url(uri);

    classloader->add_url(url);

    client->env->DeleteLocalRef(file);
    client->env->DeleteLocalRef(uri);
    client->env->DeleteLocalRef(url);
}