/* SynthStyle.java -- A set of style properties
   Copyright (C) 2006 Free Software Foundation, Inc.

This file is part of GNU Classpath.

GNU Classpath is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GNU Classpath is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU Classpath; see the file COPYING.  If not, write to the
Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
02110-1301 USA.

Linking this library statically or dynamically with other modules is
making a combined work based on this library.  Thus, the terms and
conditions of the GNU General Public License cover the whole
combination.

As a special exception, the copyright holders of this library give you
permission to link this library with independent modules to produce an
executable, regardless of the license terms of these independent
modules, and to copy and distribute the resulting executable under
terms of your choice, provided that you also meet, for each linked
independent module, the terms and conditions of the license of that
module.  An independent module is a module which is not derived from
or based on this library.  If you modify this library, you may extend
this exception to your version of the library, but you are not
obligated to do so.  If you do not wish to do so, delete this
exception statement from your version. */


package javax.swing.plaf.synth;

import java.awt.Color;
import java.awt.Font;
import java.awt.Insets;

import javax.swing.Icon;

/**
 * A set of style properties that can be installed on a component.
 *
 * @author Roman Kennke (kennke@aicas.com)
 *
 * @since 1.5
 */
public abstract class SynthStyle
{

  /**
   * Creates a new <code>SynthStyle</code> object.
   */
  public SynthStyle()
  {
    // FIXME: Implement this correctly.
  }

  public SynthGraphicsUtils getGraphicsUtils(SynthContext ctx)
  {
    // FIXME: Implement this correctly.
    return null;
  }

  public Color getColor(SynthContext ctx, ColorType type)
  {
    // FIXME: Implement this correctly.
    return null;
  }

  public abstract Color getColorForState(SynthContext ctx, ColorType type);

  public Font getFont(SynthContext ctx)
  {
    // FIXME: Implement this correctly.
    return null;
  }

  public abstract Font getFontForState(SynthContext ctx);

  public Insets getInsets(SynthContext ctx)
  {
    // FIXME: Implement this correctly.
    return null;
  }

  public SynthPainter getPainted(SynthContext ctx)
  {
    // FIXME: Implement this correctly.
    return null;
  }

  public boolean isOpaque(SynthContext ctx)
  {
    // FIXME: Implement this correctly.
    return true;
  }

  public Object get(SynthContext ctx, Object key)
  {
    // FIXME: Implement this correctly.
    return null;
  }

  public void installDefaults(SynthContext ctx)
  {
    // FIXME: Implement this correctly.
  }

  public void uninstallDefaults(SynthContext ctx)
  {
    // FIXME: Implement this correctly.
  }

  public int getInt(SynthContext ctx, Object key, int defaultValue)
  {
    // FIXME: Implement this correctly.
    return -1;
  }

  public boolean getBoolean(SynthContext ctx, Object key, boolean defaultValue)
  {
    // FIXME: Implement this correctly.
    return false;
  }

  public Icon getIcon(SynthContext ctx, Object key)
  {
    // FIXME: Implement this correctly.
    return null;
  }

  public String getString(SynthContext ctx, Object key, String defaultValue)
  {
    // FIXME: Implement this correctly.
    return null;
  }
}
