#include <jni.h>

/*
cd ~/openjdk8/jdk1.8.0_20/bin/embed2

gcc -c java.c -o java.o -m32 -I/root/openjdk8/jdk1.8.0_20/include/ -I/root/openjdk8/jdk1.8.0_20/include/linux/
gcc java.cpp -m32 -I/root/openjdk8/jdk1.8.0_20/include/ -I/root/openjdk8/jdk1.8.0_20/include/linux/ -ljvm -L/root/openjdk8/jdk1.8.0_20/jre/lib/i386/server/
LD_PRELOAD=/root/openjdk8/jdk1.8.0_20/jre/lib/i386/server/libjvm.so ./a.out
LD_LIBRARY_PATH=/root/openjdk8/jdk1.8.0_20/jre/lib/i386/server/ ./a.out

../javac Libcod.java
*/

JNIEXPORT void JNICALL JNI_OnLoad_Libcod(JNIEnv *env, jobject thisObj) {
	printf("JNI_OnLoad_Libcod\n");
	printf("JNI_OnLoad_Libcod\n");
	printf("JNI_OnLoad_Libcod\n");
	printf("JNI_OnLoad_Libcod\n");
}
 
JNIEXPORT void JNICALL Java_Libcod_sayHello(JNIEnv *env, jobject thisObj) {
	printf("NATIVE FUNCTION CALLELD\n");
}

int embed_java() {
	JavaVM *jvm = NULL;
	JNIEnv *env = NULL;
	JavaVMInitArgs vmArgs;
	JavaVMOption options[2]; int n = 0;
	//options[n++].optionString = "jdk.nashorn.tools.Shell";
	options[n++].optionString = "-Djava.class.path=.";
	options[n++].optionString = "-Djava.library.path=.";

	vmArgs.version = JNI_VERSION_1_8;
	vmArgs.nOptions = n;
	vmArgs.options = options;
	vmArgs.ignoreUnrecognized = 0;

	//printf("dlsym(\"JNI_CreateJavaVM\") = %.8p\n", dlsym(NULL, "JNI_CreateJavaVM"));
	
	JNI_CreateJavaVM(&jvm, (void**)&env, &vmArgs);
	printf("[JAVA] jvm=%.8p env=%.8p\n", jvm, env);
	
	
	jclass class_shell;
	jmethodID method_main;

	char *init_class;
	init_class = "jdk/nashorn/tools/Shell"; // would load the jjs-REPL-Shell
	init_class = "Init";
	class_shell = (*env)->FindClass(env, init_class);
	if ( ! class_shell) {
		printf("[JAVA] Could not find class: %s\n", init_class);
		return 0;
	}
	printf("[JAVA] class_shell: %p\n", class_shell);
	
	
	static JNINativeMethod methods[] = {
		{"sayHello",    "()V",                    (void *)&Java_Libcod_sayHello},
		//{"wait",        "(J)V",                   (void *)&JVM_MonitorWait},
		//{"notify",      "()V",                    (void *)&JVM_MonitorNotify},
		//{"notifyAll",   "()V",                    (void *)&JVM_MonitorNotifyAll},
		//{"clone",       "()Ljava/lang/Object;",   (void *)&JVM_Clone},
	};

	(*env)->RegisterNatives(env, class_shell, methods, sizeof(methods)/sizeof(methods[0]));
	
	method_main = (*env)->GetStaticMethodID(env, class_shell, "main", "([Ljava/lang/String;)V");
	if ( ! method_main) {
		printf("[JAVA] Could not find static method: main(String[])\n");
		return 0;
	}
	printf("[JAVA] method_main: %.8p\n", method_main);
	
	jobjectArray applicationArgs;
	jstring applicationArg0;
	applicationArgs = (*env)->NewObjectArray(env, 1, (*env)->FindClass(env, "java/lang/String"), NULL);
	applicationArg0 = (*env)->NewStringUTF(env, "From-C-program");
	(*env)->SetObjectArrayElement(env, applicationArgs, 0, applicationArg0);
	(*env)->CallStaticVoidMethod(env, class_shell, method_main);


	//jvm->DestroyJavaVM();

	return 0;
}