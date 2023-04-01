#include "j_classloader.hpp"
#include "injector.hpp"

#include <stdexcept>

j_classloader::j_classloader(jobject classloader)
{
	this->classloader = classloader;

	jclass c_urlclassloader = client->env->FindClass("java/net/URLClassLoader");

	if (!client->env->IsInstanceOf(classloader, c_urlclassloader))
	{
		throw std::invalid_argument("classloader must be an (instance of) URLClassLoader");
	}

	this->m_findClass = client->env->GetMethodID(c_urlclassloader, "findClass", "(Ljava/lang/String;)Ljava/lang/Class;");
	this->m_addURL = client->env->GetMethodID(c_urlclassloader, "addURL", "(Ljava/net/URL;)V");

	client->env->DeleteLocalRef(c_urlclassloader);
}

j_classloader::~j_classloader()
{
	client->env->DeleteLocalRef(this->classloader);
}

jclass j_classloader::find_class(const char* class_name)
{
	jclass clazz = nullptr;
	jstring j_class_name = client->env->NewStringUTF(class_name);

	clazz = reinterpret_cast<jclass>(client->env->CallObjectMethod(this->classloader, m_findClass, j_class_name));
	client->env->DeleteLocalRef(j_class_name);

	return clazz;
}

void j_classloader::add_url(jobject url)
{
	client->env->CallVoidMethod(this->classloader, m_addURL, url);
}