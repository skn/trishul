package org.kaffe.beans.editors;

import java.beans.PropertyEditorSupport;

/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
public class FloatEditor
  extends PropertyEditorSupport
{
public String getAsText()
	{
	return (getValue().toString());
}

public String getJavaInitializationString()
	{
	return (getValue().toString() + "f");
}

public void setAsText(String text) throws IllegalArgumentException
{
	try {
		setValue(new Float(text));
	}
	catch (NumberFormatException _) {
		throw new IllegalArgumentException(text);
	}
}
}
