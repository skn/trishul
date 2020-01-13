/* SwingDemo.java -- An example of using the javax.swing UI.
   Copyright (C) 2003, 2004, 2005, 2006  Free Software Foundation, Inc.

This file is part of GNU Classpath examples.

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
*/


package gnu.classpath.examples.swing;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;
import javax.swing.tree.*;

import javax.swing.plaf.metal.DefaultMetalTheme;
import javax.swing.plaf.metal.MetalLookAndFeel;
import javax.swing.plaf.metal.MetalTheme;
import javax.swing.plaf.metal.OceanTheme;

import java.net.URL;

public class Demo
{
  JFrame frame;

  /**
   * The main desktop. This is package private to avoid synthetic accessor
   * method.
   */
  JDesktopPane desktop;

  /**
   * The themes menu. This is implemented as a field so that the L&F switcher
   * can disable the menu when a non-Metal L&F is selected.
   */
  JMenu themesMenu;

  static Color blueGray = new Color(0xdc, 0xda, 0xd5);

  private static Icon stockIcon(String s)
  {
    return getIcon("/gnu/classpath/examples/icons/stock-" + s + ".png", s);
  }

  static Icon bigStockIcon(String s)
  {
    return getIcon("/gnu/classpath/examples/icons/big-" + s + ".png", s);
  }

  private static Icon getIcon(String location, String name)
  {
    URL url = Demo.class.getResource(location);
    if (url == null) System.err.println("WARNING " + location + " not found.");
    return new ImageIcon(url, name);
  }

  private JMenuBar mkMenuBar()
  {
    JMenuBar bar = new JMenuBar();
    
    JMenu file = new JMenu("File");
    JMenu edit = new JMenu("Edit");
    JMenu help = new JMenu("Help");

    file.setMnemonic(KeyEvent.VK_F);
    edit.setMnemonic(KeyEvent.VK_E);
    help.setMnemonic(KeyEvent.VK_H);

    file.add(new JMenuItem("New", stockIcon("new")));
    file.add(new JMenuItem("Open", stockIcon("open")));

    JMenu recent = new JMenu("Recent Files...");
    recent.add(new JMenuItem("war-and-peace.txt"));
    recent.add(new JMenuItem("taming-of-shrew.txt"));
    recent.add(new JMenuItem("sun-also-rises.txt"));
    file.add(recent);
    file.add(new JMenuItem("Save", stockIcon("save")));
    file.add(new JMenuItem("Save as...", stockIcon("save-as")));

    JMenuItem exit = new JMenuItem("Exit", stockIcon("quit"));
    exit.addActionListener(new ActionListener()
      {
        public void actionPerformed(ActionEvent e)
        {
          System.exit(1);
        }
      });

    file.add(exit);

    edit.add(new JMenuItem("Cut", stockIcon("cut")));
    edit.add(new JMenuItem("Copy", stockIcon("copy")));
    edit.add(new JMenuItem("Paste", stockIcon("paste")));

    JMenu preferences = new JMenu("Preferences...");
    preferences.add(new JCheckBoxMenuItem("Microphone Active",
		    stockIcon("mic")));
    preferences.add(new JCheckBoxMenuItem("Check Spelling",
		    stockIcon("spell-check")));
    preferences.add(new JCheckBoxMenuItem("World Peace"));
    preferences.add(new JSeparator());
    preferences.add(new JRadioButtonMenuItem("Radio Button"));
    edit.add(preferences);

    JMenu examples = new JMenu("Examples");
    examples.add(new JMenuItem(new PopupAction("Buttons",
                                             ButtonDemo.createDemoFactory())));
    examples.add(new JMenuItem(new PopupAction("Slider",
                                             SliderDemo.createDemoFactory())));
    examples.add(new JMenuItem(new PopupAction("ProgressBar",
                                        ProgressBarDemo.createDemoFactory())));
    examples.add(new JMenuItem(new PopupAction("Scrollbar",
                                          ScrollBarDemo.createDemoFactory())));
    examples.add(new JMenuItem(new PopupAction("Spinner",
                                            SpinnerDemo.createDemoFactory())));
    examples.add(new JMenuItem(new PopupAction("TextField",
                                          TextFieldDemo.createDemoFactory())));
    examples.add(new JMenuItem(new PopupAction("TextArea",
                                           TextAreaDemo.createDemoFactory())));
    examples.add(new JMenuItem(new PopupAction("FileChooser",
                                        FileChooserDemo.createDemoFactory())));

    examples.add(new JMenuItem(new PopupAction("ComboBox",
                                           ComboBoxDemo.createDemoFactory())));

    examples.add(new JMenuItem(new PopupAction("Table",
                                              TableDemo.createDemoFactory())));
    examples.add(new JMenuItem(new PopupAction("List",
                                               ListDemo.createDemoFactory())));
    examples.add(new JMenuItem(new PopupAction("TabbedPane",
                                         TabbedPaneDemo.createDemoFactory())));
    examples.add(new JMenuItem(new PopupAction("Tree",
                                               TreeDemo.createDemoFactory())));
    examples.add(new JMenuItem(new PopupAction("Theme Editor",
                                       MetalThemeEditor.createDemoFactory())));

    final JMenuItem vmMenu;
    
    help.add(new JMenuItem("just play with the widgets"));
    help.add(new JMenuItem("and enjoy the sensation of"));
    help.add(new JMenuItem("your neural connections growing"));
    help.add(new JSeparator());
    help.add(vmMenu = new JMenuItem("Really, which VM is this running on?"));
    vmMenu.addActionListener(new ActionListener()
      {
        public void actionPerformed(ActionEvent ae)
          {
            String message = "This is "
                             + System.getProperty("java.vm.name")
                             + " Version "
                             + System.getProperty("java.vm.version")
                             + " distributed by "
                             + System.getProperty("java.vm.vendor");
                         
            String gnuClasspath = System.getProperty("gnu.classpath.version");
            if(gnuClasspath != null)
              message += "\nThe runtime's libraries are "
                         + "kindly provided by the "
                         + "members of GNU Classpath and are in version "
                         + gnuClasspath + ".";
                         
                         JOptionPane.showMessageDialog(vmMenu, message);
            }
      });

    // Create L&F menu.
    JMenu lafMenu = new JMenu("Look and Feel");
    ButtonGroup lafGroup = new ButtonGroup();
    UIManager.LookAndFeelInfo[] lafs = UIManager.getInstalledLookAndFeels();
    String currentLaf = UIManager.getLookAndFeel().getClass().getName();
    for (int i = 0; i < lafs.length; ++i)
      {
        UIManager.LookAndFeelInfo laf = lafs[i];
        ChangeLAFAction action = new ChangeLAFAction(laf);
        JRadioButtonMenuItem lafItem = new JRadioButtonMenuItem(action);
        boolean selected = laf.getClassName().equals(currentLaf);
        lafItem.setSelected(selected);
        lafMenu.add(lafItem);
      }

    // Create themes menu.
    themesMenu = new JMenu("Themes");
    ButtonGroup themesGroup = new ButtonGroup();
    JRadioButtonMenuItem ocean =
      new JRadioButtonMenuItem(new ChangeThemeAction(new OceanTheme()));
    ocean.setSelected(MetalLookAndFeel.getCurrentTheme() instanceof OceanTheme);
    themesMenu.add(ocean);
    themesGroup.add(ocean);
    JRadioButtonMenuItem steel =
      new JRadioButtonMenuItem(new ChangeThemeAction(new DefaultMetalTheme()));
    ocean.setSelected(MetalLookAndFeel.getCurrentTheme()
                      instanceof DefaultMetalTheme);
    themesMenu.add(steel);
    themesGroup.add(steel);
    
    bar.add(file);
    bar.add(edit);
    bar.add(examples);
    bar.add(lafMenu);
    bar.add(themesMenu);
    bar.add(help);
    return bar;
  }

  private static void triggerDialog(final JButton but, final String dir)
  {
    but.addActionListener(new ActionListener()
      {
        public void actionPerformed(ActionEvent e)
        {
          JOptionPane.showConfirmDialog(but, 
                                        "Sure you want to go " + dir + "?",
                                        "Confirm",
                                        JOptionPane.OK_CANCEL_OPTION, 
                                        JOptionPane.QUESTION_MESSAGE,
                                        bigStockIcon("warning"));
        }
      });
  }

  static JToolBar mkToolBar()
  {
    JToolBar bar = new JToolBar();

    JButton b = mkButton(stockIcon("go-back"));
    triggerDialog(b, "back");
    bar.add(b);

    b = mkButton(stockIcon("go-down"));
    triggerDialog(b, "down");
    bar.add(b);

    b = mkButton(stockIcon("go-forward"));
    triggerDialog(b, "forward");
    bar.add(b);
    return bar;
  }

  private static String valign2str(int a)
  {
    switch (a)
      {
      case SwingConstants.CENTER:
        return "Center";
      case SwingConstants.TOP:
        return "Top";
      case SwingConstants.BOTTOM:
        return "Bottom";
      default:
        return "Unknown";
      }
  }

  static String halign2str(int a)
  {
    switch (a)
      {
      case SwingConstants.CENTER:
        return "Center";
      case SwingConstants.RIGHT:
        return "Right";
      case SwingConstants.LEFT:
        return "Left";
      default:
        return "Unknown";
      }
  }

  private static JButton mkButton(String title, Icon icon, 
                                  int hAlign, int vAlign,
                                  int hPos, int vPos)
  {    
    JButton b;
    if (icon == null)
      b = new JButton(title);
    else if (title == null)
      b = new JButton(icon);
    else
      b = new JButton(title, icon);
    
    b.setToolTipText(title);
    if (hAlign != -1) b.setHorizontalAlignment(hAlign);
    if (vAlign != -1) b.setVerticalAlignment(vAlign);
    if (hPos != -1) b.setHorizontalTextPosition(hPos);
    if (vPos != -1) b.setVerticalTextPosition(vPos);      
    return b;
  }

  static JButton mkButton(String title)
  {
    return mkButton(title, null, -1, -1, -1, -1);
  }

  static JButton mkButton(Icon i)
  {
    return mkButton(null, i, -1, -1, -1, -1);
  }


  private static JScrollPane mkScrollPane(JComponent inner)
  {
    JScrollPane jsp;
    jsp = new JScrollPane(inner,
			  JScrollPane.VERTICAL_SCROLLBAR_AS_NEEDED, 
			  JScrollPane.HORIZONTAL_SCROLLBAR_AS_NEEDED);
    
    return jsp;
  }

  public Demo()
  {
    frame = new JFrame("Swing Activity Board");
    frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
    frame.setJMenuBar(mkMenuBar());
    JComponent component = (JComponent) frame.getContentPane();
    component.setLayout(new BorderLayout());
    component.add(mkToolBar(), BorderLayout.NORTH);
    JPanel main = new JPanel();
    main.setLayout(new BoxLayout(main, BoxLayout.Y_AXIS));
    desktop = createDesktop();
    main.add(desktop);
    main.add(mkButtonBar());
    component.add(main, BorderLayout.CENTER);
    frame.pack();
    frame.show();
  }

  public static class LaterMain
    implements Runnable
  {
    public void run()
    {
      Demo demo = new Demo();      
    }
  }

  public static void main(String args[])
  {
    SwingUtilities.invokeLater(new LaterMain());
  }

  private static JButton mkBigButton(String title)
  {
    JButton b = new JButton(title);
    b.setMargin(new Insets(5,5,5,5));
    return b;
  }

  private static JPanel mkPanel(JComponent[] inners)
  {
    JPanel p = new JPanel();
    for (int i = 0; i < inners.length; ++i)
      {
        p.add(inners[i]);
      }
    return p;
  }

  static JButton mkDisposerButton(final JFrame c)
  {
    JButton close = mkBigButton("Close");
    close.addActionListener(new ActionListener()
      {
        public void actionPerformed(ActionEvent e)
        {
          c.dispose();
        }
      });
    return close;
  }

  public static JColorChooser mkColorChooser()
  {
    return new JColorChooser();
  }

  /**
   * This action brings up a new Window with the specified content.
   */
  private class PopupAction
    extends AbstractAction
  {
    /**
     * The component to be shown.
     */
    private DemoFactory demoFactory;

    /**
     * Creates a new PopupAction with the specified name and showing the
     * component created by the specified DemoFactory when activated.
     *
     * @param n the name of the action
     * @param factory the demo factory
     */
    PopupAction(String n, DemoFactory factory)
    {
      putValue(NAME, n);
      demoFactory = factory;
    }

    /**
     * Brings up the new window showing the component stored in the
     * constructor.
     *
     * @param e the action event that triggered the action
     */
    public void actionPerformed(ActionEvent e)
    {
      JInternalFrame frame = new JInternalFrame((String) getValue(NAME));
      frame.setClosable(true);
      frame.setIconifiable(true);
      frame.setMaximizable(true);
      frame.setResizable(true);
      frame.setContentPane(demoFactory.createDemo());
      frame.pack();
      desktop.add(frame);
      frame.setVisible(true);
    }
  }

  /**
   * Create the tree.
   * 
   * @return thr scroll pane, containing the tree.
   */
  private static JComponent mkTree()
  {
    DefaultMutableTreeNode root = new DefaultMutableTreeNode("Root node");
    
    addChildren("Node", root, 12);

    JTree tree = new JTree(root);
    tree.setLargeModel(true);
    DefaultTreeSelectionModel dtsm = new DefaultTreeSelectionModel();
    dtsm.setSelectionMode(DefaultTreeSelectionModel.SINGLE_TREE_SELECTION);
    tree.setSelectionModel(dtsm);
    
    // Make it editable.
    tree.setEditable(true);
    
    JComponent t = mkScrollPane(tree);
    t.setPreferredSize(new Dimension(200,200));
    return t;
  }
  
  /**
   * Add the specified number of children to this parent node. For each
   * child, the method is called recursively adding the nChildren-3 number of 
   * grandchildren.
   * 
   * @param parent the parent node
   * @param nChildren the number of children
   */
  private static void addChildren(String name, DefaultMutableTreeNode parent,
                                                int nChildren)
  {
    for (int i = 0; i < nChildren; i++)
      {
        String child_name = parent+"."+i;
        DefaultMutableTreeNode child = new DefaultMutableTreeNode
         (child_name);
        parent.add(child);
        addChildren(child_name, child, nChildren-3);
      }
  }
  
  private JPanel mkButtonBar()
  {    
    JPanel panel = new JPanel(new GridLayout(3, 1, 5, 5));
    panel.add(new JButton(new PopupAction("Buttons",
                                          ButtonDemo.createDemoFactory())));
    panel.add(new JButton(new PopupAction("Slider",
                                          SliderDemo.createDemoFactory())));
    panel.add(new JButton(new PopupAction("ProgressBar",
                                        ProgressBarDemo.createDemoFactory())));
    panel.add(new JButton(new PopupAction("ScrollBar",
                                          ScrollBarDemo.createDemoFactory())));
    panel.add(new JButton(new PopupAction("Spinner",
                                          SpinnerDemo.createDemoFactory())));
    panel.add(new JButton(new PopupAction("TextField",
                                          TextFieldDemo.createDemoFactory())));
    panel.add(new JButton(new PopupAction("TextArea",
                                          TextAreaDemo.createDemoFactory())));
    panel.add(new JButton(new PopupAction("FileChooser",
                                        FileChooserDemo.createDemoFactory())));
    panel.add(new JButton(new PopupAction("ComboBox",
                                          ComboBoxDemo.createDemoFactory())));
    panel.add(new JButton(new PopupAction("Table",
                                          TableDemo.createDemoFactory())));
    panel.add(new JButton(new PopupAction("List",
                                          ListDemo.createDemoFactory())));
    panel.add(new JButton(new PopupAction("TabbedPane",
                                         TabbedPaneDemo.createDemoFactory())));
    panel.add(new JButton(new PopupAction("Tree",
                                          TreeDemo.createDemoFactory())));
    panel.add(new JButton(new PopupAction("Theme Editor",
                                       MetalThemeEditor.createDemoFactory())));
    JButton exitDisposer = mkDisposerButton(frame);
    panel.add(exitDisposer);
    
    panel.setMaximumSize(new Dimension(Integer.MAX_VALUE, 
                                       panel.getPreferredSize().height));
    exitDisposer.addActionListener(new ActionListener()
      {
	public void actionPerformed(ActionEvent e)
	{
	  System.exit(1);
	}
      });
    return panel;
  }

  /**
   * Creates and returns the main desktop.
   *
   * @return the main desktop
   */
  private JDesktopPane createDesktop()
  {
    JDesktopPane d = new DemoDesktop();
    d.setPreferredSize(new Dimension(900, 500));
    return d;
  }

  /**
   * This Action is used to switch Metal themes.
   */
  class ChangeThemeAction extends AbstractAction
  {
    /**
     * The theme to switch to.
     */
    MetalTheme theme;

    /**
     * Creates a new ChangeThemeAction for the specified theme.
     *
     * @param t the theme to switch to
     */
    ChangeThemeAction(MetalTheme t)
    {
      theme = t;
      putValue(NAME, t.getName());
    }

    /**
     * Changes the theme to the one specified in the constructor.
     *
     * @param event the action event that triggered this action
     */
    public void actionPerformed(ActionEvent event)
    {
      MetalLookAndFeel.setCurrentTheme(theme);
      try
        {
          // Only switch theme if we have a metal L&F. It is still necessary
          // to install a new MetalLookAndFeel instance.
          if (UIManager.getLookAndFeel() instanceof MetalLookAndFeel)
            UIManager.setLookAndFeel(new MetalLookAndFeel());
        }
      catch (UnsupportedLookAndFeelException ex)
        {
          ex.printStackTrace();
        }
      SwingUtilities.updateComponentTreeUI(frame);
    }
    
  }

  /**
   * This Action is used to switch Metal themes.
   */
  class ChangeLAFAction extends AbstractAction
  {
    /**
     * The theme to switch to.
     */
    private UIManager.LookAndFeelInfo laf;

    /**
     * Creates a new ChangeLAFAction for the specified L&F.
     *
     * @param l the L&F to switch to
     */
    ChangeLAFAction(UIManager.LookAndFeelInfo l)
    {
      laf = l;
      putValue(NAME, laf.getName());
    }

    /**
     * Changes the theme to the one specified in the constructor.
     *
     * @param event the action event that triggered this action
     */
    public void actionPerformed(ActionEvent event)
    {
      try
        {
          UIManager.setLookAndFeel(laf.getClassName());
        }
      catch (Exception ex)
        {
          ex.printStackTrace();
        }
      SwingUtilities.updateComponentTreeUI(frame);
      themesMenu.setEnabled(laf.getClassName()
                           .equals("javax.swing.plaf.metal.MetalLookAndFeel"));
    }
    
  }
}
