package com.sasamp.launcher.utils;

import java.io.File;

public class FileUtils {
	public static void delete(File file) {
        if (file.exists()) {
            if (file.isDirectory()) {
                for (File f : file.listFiles()) {
                    delete(f);
                }
            }
            file.delete();
        }
    }
}