# Blender.Text3 module and the Text3d PyType object

"""
The Blender.Text3d.Font subsubmodule.

Text3d.Font Objects
===================

This module provides access to B{Font} objects in Blender.

Example::
  import Blender
  from Blender import Window, Curve, Object, Scene, Text3d
  #
  txt = Text3d.New("MyText")  # create a new Text3d object called MyText
  cur = Scene.getCurrent()    # get current scene
  ob = Object.New('Text')     # make curve object
  fnt_obj= Text3d.LoadFont('/home/joilnen/c0419bt_.pfb') # here a valid font file
  txt.setFont(fnt_obj)
  ob.link(txt)                # link curve data with this object
  cur.link(ob)                # link object into scene
  ob.makeDisplayList()        # rebuild the display list for this object
  Window.RedrawAll()
"""

def New (name = None):
  """
  Create a new Text3d.Font object.
  @type name: string
  @param name: file of the font
  @rtype: Blender Text3d.Font
  @return: The created Text3d.Font Data object.
  """

def Get (name = None):
  """
  Get the Text3d object(s) from Blender.
  @type name: string
  @param name: The name of the Text3d object.
  @rtype: Blender Text3d or a list of Blender Text3ds
  @return: It depends on the 'name' parameter:
      - (name): The Text3d object with the given name;
      - ():     A list with all Text3d objects in the current scene.
  """
class Font:
  """
  The Text3d.Font object
  ======================
    This object gives access  Blender's B{Font} objects
  @ivar name: The Text3d name.
  @ivar filename: The filename of the file loaded into this Text.
  @ivar mode: The follow_mode flag: if 1 it is 'on'; if 0, 'off'.
  @ivar nlines: The number of lines in this Text.
  """

  def getName():
    """
    Get the name of this Text3d object.
    @rtype: string
    """

  def setName( name ):
    """
    Set the name of this Text3d object.
    @type name: string
    @param name: The new name.
    @returns: PyNone
    """

  def getText():
    """
    Get text string for this object
    @rtype: string
    """

  def setText( name ):
    """
    Set the text string in this Text3d object
    @type name: string
    @param name:  The new text string for this object.
    @returns: PyNone
    """

