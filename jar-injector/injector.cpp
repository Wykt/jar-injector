#include "injector.hpp"

std::unique_ptr<c_client> client;

DWORD injector::main_thread(HMODULE hModule)
{
	JavaVM* jvm = utils::get_jvm_instance();

	if (!jvm)
	{
		ERROR_LOG("Couldn't find JVM instance");
		FreeLibraryAndExitThread(hModule, 1);
	}

	client = std::make_unique<c_client>(jvm);
	client->initialize();

	if (!client->env)
	{
		ERROR_LOG("Couldn't find JNIEnv");
		FreeLibraryAndExitThread(hModule, 2);
	}

	if (!client->jvmti)
	{
		ERROR_LOG("Couldn't find jvmtiEnv");
		FreeLibraryAndExitThread(hModule, 3);
	}

	utils::add_file_path_to_url_classloader(client->classloader, JAR_PATH);

	jclass c_client_main = client->classloader->find_class(JAR_MAIN_CLASS);
	jmethodID m_client_main = client->env->GetStaticMethodID(c_client_main, JAR_MAIN_METHOD, "()V");

	client->env->CallStaticVoidMethod(c_client_main, m_client_main);

	client->env->DeleteLocalRef(c_client_main);
	
	FreeLibraryAndExitThread(hModule, 0);
}