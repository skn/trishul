/* FixedHeightLayoutCache.java --
Copyright (C) 2002, 2004, 2006,  Free Software Foundation, Inc.

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

package javax.swing.tree;

import java.awt.Rectangle;
import java.util.Enumeration;

import javax.swing.event.TreeModelEvent;

/**
 * FixedHeightLayoutCache
 * 
 * @author Andrew Selkirk
 */
public class FixedHeightLayoutCache
		extends AbstractLayoutCache
{

  /**
   * Constructor FixedHeightLayoutCache
   */
  public FixedHeightLayoutCache()
  {
    // TODO
  } 

  /**
   * getRowCount
   * 
   * @return int
   */
  public int getRowCount() 
  {
    return 0; // TODO
  }  

  /**
   * invalidatePathBounds
   * 
   * @param value0 TODO
   */
  public void invalidatePathBounds(TreePath value0)
  {
    // TODO
  } 

  /**
   * invalidateSizes
   */
  public void invalidateSizes()
  {
    // TODO
  } 

  /**
   * isExpanded
   * 
   * @param value0 TODO
   * @return boolean
   */
  public boolean isExpanded(TreePath value0)
  {
    return false; // TODO
  } 

  /**
   * getBounds
   * 
   * @param value0 TODO
   * @param value1 TODO
   * @return Rectangle
   */
  public Rectangle getBounds(TreePath value0, Rectangle value1)
  {
    return null; // TODO
  } 

  /**
   * getPathForRow
   * 
   * @param row TODO
   * @return TreePath
   */
  public TreePath getPathForRow(int row)
  {
    //TODO
    return null;
  } 

  /**
   * getRowForPath
   * 
   * @param value0 TODO
   * @return int
   */
  public int getRowForPath(TreePath value0)
  {
    return 0;
  } 

  /**
   * getPathClosestTo
   * 
   * @param value0 TODO
   * @param value1 TODO
   * @return TreePath
   */
  public TreePath getPathClosestTo(int value0, int value1)
  {
    return null; // TODO
  } 

  /**
   * getVisibleChildCount
   * 
   * @param value0 TODO
   * @return int
   */
  public int getVisibleChildCount(TreePath value0)  
  {
    return 0; // TODO
  } 

  /**
   * getVisiblePathsFrom
   * 
   * @param value0 TODO
   * @return Enumeration
   */
  public Enumeration getVisiblePathsFrom(TreePath value0)
  {
    return null; // TODO
  }

  /**
   * setExpandedState
   * 
   * @param value0 TODO
   * @param value1 TODO
   */
  public void setExpandedState(TreePath value0, boolean value1)
  {
    // TODO
  } 

  /**
   * getExpandedState
   * 
   * @param value0 TODO
   * @return boolean
   */
  public boolean getExpandedState(TreePath value0)
  {
    return false; // TODO
  }

  /**
   * treeNodesChanged
   * 
   * @param value0 TODO
   */
  public void treeNodesChanged(TreeModelEvent value0)
  {
    // TODO
  } 

  /**
   * treeNodesInserted
   * 
   * @param value0 TODO
   */
  public void treeNodesInserted(TreeModelEvent value0)
  {
    // TODO
  } 

  /**
   * treeNodesRemoved
   * 
   * @param value0 TODO
   */
  public void treeNodesRemoved(TreeModelEvent value0)
  {
    // TODO
  } 

  /**
   * treeStructureChanged
   * 
   * @param value0 TODO
   */
  public void treeStructureChanged(TreeModelEvent value0)
  {
    // TODO
  } 

}