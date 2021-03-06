/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package org.kaffe.io;

import java.io.IOException;
import java.io.OutputStream;

public class StdOutputStream
  extends OutputStream {

static {
        System.loadLibrary("javaio");
}

public StdOutputStream() {
}

native public void write(int b) throws IOException;

}
