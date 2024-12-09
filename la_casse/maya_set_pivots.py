

# локатры должны быть приперенчены к объекту

# первая кнопка на полке
import maya.cmds as cmds
loc1 = cmds.xform('locator1', q=1, t=True, ws=True)
cmds.move(loc1[0], loc1[1], loc1[2], "pCube1.scalePivot", "pCube1.rotatePivot", absolute=True)


# вторая кнопка на полке
import maya.cmds as cmds
loc2 = cmds.xform('locator2', q=1, t=True, ws=True)
cmds.move(loc2[0], loc2[1], loc2[2], "pCube1.scalePivot", "pCube1.rotatePivot", absolute=True)
