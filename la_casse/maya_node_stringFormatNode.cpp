//-
// ==========================================================================
// Copyright 1995,2006,2008 Autodesk, Inc. All rights reserved.
//
// Use of this software is subject to the terms of the Autodesk
// license agreement provided at the time of installation or download,
// or which otherwise accompanies this software in either electronic
// or hard copy form.
// ==========================================================================
//+
// Example Plugin: stringFormatNode.cpp
//
// This plug-in is an example of a user-defined dependency graph node.
// It takes several numbers as input, as well as a format string. It
// generates a formatted string as an output.
//
// All occurences of ^[0-9][dft] are replaced with the corresponding
// input parameter using the specified format. 
//
// d means output as an integer.
// f means output as a floating point value.
// t means output as a timecode.
// 
// For example, the string "format ^1f ^2d ^0t end format", using the
// array {2.3, 3.4, 4.5} as an input will generate the result
// "format 3.4 4 00:00:02 end format"
//
#include <math.h>
#include <maya/MIOStream.h>
#include <maya/MPxNode.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnStringData.h>
#include <maya/MFnPlugin.h>
// The stringFormat class defines the attributes and methods necessary
// for the stringFormatNode plugin
//
class stringFormat : public MPxNode
{
public:
                                                stringFormat();
        virtual                         ~stringFormat();
        virtual MStatus         compute( const MPlug& plug, MDataBlock& data );
        static  void*           creator();
        static  MStatus         initialize();
public:
        static  MTypeId         id;                                     // Unique node Id
        static  MObject         attrFormat;                     // Formatting string
        static  MObject         attrValues;                     // Input values (array)
        static  MObject         attrOutput;                     // Result
};
MTypeId     stringFormat::id( 0x81034 );
MObject     stringFormat::attrFormat;        
MObject     stringFormat::attrValues;       
MObject     stringFormat::attrOutput;       
// The creator method creates an instance of the stringFormat class
// and is the first method called by Maya when a stringFormat node
// needs to be created.
//
void* stringFormat::creator()
{
        return new stringFormat();
}
// The initialize routine is called after the node has been created.
// It sets up the input and output attributes and adds them to the
// node. Finally the dependencies are arranged so that when the
// inputs change Maya knowns to call compute to recalculate the output
// value.
//
MStatus stringFormat::initialize()
{
        MFnNumericAttribute numAttr;
        MFnTypedAttribute       typedAttr;
        MFnStringData           stringData;
        MStatus                         stat;
        MStatus                         stat2;
        // Setup the input attributes
        //
        attrFormat = typedAttr.create("format", "f", MFnData::kString, stringData.create(&stat2), &stat);
        CHECK_MSTATUS( stat2 );
        CHECK_MSTATUS( stat );
        CHECK_MSTATUS( typedAttr.setStorable( true ) );
        CHECK_MSTATUS( typedAttr.setKeyable( true ) );
        attrValues = numAttr.create("values", "v", MFnNumericData::kDouble, 0, &stat);
        CHECK_MSTATUS( stat );
        CHECK_MSTATUS( numAttr.setArray( true ) );
        CHECK_MSTATUS( numAttr.setReadable( false ) );
        CHECK_MSTATUS( numAttr.setIndexMatters( true ) );
        CHECK_MSTATUS( numAttr.setStorable( true ) );
        CHECK_MSTATUS( numAttr.setKeyable( true ) );
        attrOutput = typedAttr.create( "output", "o", MFnData::kString, stringData.create(&stat2), &stat);
        CHECK_MSTATUS( stat2 );
        CHECK_MSTATUS( stat );
        CHECK_MSTATUS( typedAttr.setWritable( false ) );
        CHECK_MSTATUS( typedAttr.setStorable( false ) );
        // Add the attributes to the node
        //
        CHECK_MSTATUS( addAttribute( attrFormat ) );
        CHECK_MSTATUS( addAttribute( attrValues ) );
        CHECK_MSTATUS( addAttribute( attrOutput ) );
        // Set the attribute dependencies
        //
        CHECK_MSTATUS( attributeAffects( attrFormat, attrOutput ) );
        CHECK_MSTATUS( attributeAffects( attrValues, attrOutput ) );
        return MS::kSuccess;
} 
// The constructor does nothing
//
stringFormat::stringFormat() {}
// The destructor does nothing
//
stringFormat::~stringFormat() {}
// The compute method is called by Maya when the input values
// change and the output values need to be recomputed.
// The input values are read then using sinf() and cosf()
// the output values are stored on the output plugs.
//
// Look for ^[0-9][a-z] after the given index. Returns the position of
// the characted just after '^'
int findNextMatch(MString & string, int indx, int & param, char & letter)
{
        // Warning this is not I18N compliant
        const char * str = string.asChar();
        while (str[indx]) {
                if ((str[indx] == '^') && 
                        (str[indx+1] >= '0') && (str[indx+1] <= '9') &&
                        (str[indx+2] >= 'a') && (str[indx+2] <= 'z')) {
                        param = str[indx+1] - '0';
                        letter = str[indx+2];
                        return indx+1;
                }
                indx++;
        }
        return -1;
}
MStatus stringFormat::compute (const MPlug& plug, MDataBlock& data)
{
        MStatus status;
        // Check that the requested recompute is one of the output values
        //
        if (plug == attrOutput) {
                // Read the input values
                //
                MDataHandle inputData = data.inputValue (attrFormat, &status);
                CHECK_MSTATUS( status );
                MString format = inputData.asString();
                // Get input data handle, use outputArrayValue since we do not
                // want to evaluate all inputs, only the ones related to the
                // requested multiIndex. This is for efficiency reasons.
                //
                MArrayDataHandle vals = data.outputArrayValue(attrValues, &status);
                CHECK_MSTATUS( status );
                int indx = 0;
                int param;
                char letter;
                while ((indx = findNextMatch(format, indx, param, letter)) > 0) {
                        double val = 0.;
                        status = vals.jumpToElement(param);
                        if (status == MStatus::kSuccess) {
                                MDataHandle thisVal = vals.inputValue( &status );
                                if (status == MStatus::kSuccess) {
                                        val = thisVal.asDouble();
                                }
                        }
                        MString replace;
                        bool valid = false;
                        switch (letter) {
                                case 'd':                                       // Integer
                                val = floor(val+.5);
                                // No break here
                                case 'f':                                       // Float
                                replace.set(val);
                                valid = true;
                                break;
                                case 't':                                       // Timecode
                                {
                                        const char * sign = "";
                                        if (val<0) {
                                                sign = "-";
                                                val = -val;
                                        }
                                        int valInt = (int)(val+.5);
                                        int sec = valInt / 24;
                                        int frame = valInt - sec * 24;
                                        int min = sec / 60;
                                        sec -= min * 60;
                                        int hour = min / 60;
                                        min -= hour * 60;
                                        char buffer[90];
                                        if (hour>0)
                                                sprintf(buffer, "%s%d:%02d:%02d.%02d", sign, hour, min, sec, frame);
                                        else
                                                sprintf(buffer, "%s%02d:%02d.%02d", sign, min, sec, frame);
                                        replace = buffer;
                                }
                                valid = true;
                                break;
                        }
                        if (valid) {
                                format = format.substring(0, indx-2) + replace + format.substring(indx+2, format.length()-1);
                                indx += replace.length() - 3;
                        }
                }
                // Store the result
                //
                MDataHandle output = data.outputValue(attrOutput, &status );
                CHECK_MSTATUS( status );
                output.set( format );
        } else {
                return MS::kUnknownParameter;
        }
        return MS::kSuccess;
}
// The initializePlugin method is called by Maya when the plugin is
// loaded. It registers the node and the UI.
//
MStatus initializePlugin ( MObject obj )
{ 
        MStatus   status;
        MFnPlugin plugin( obj, PLUGIN_COMPANY, "8.0", "Any");
        // Register the node
        CHECK_MSTATUS_AND_RETURN_IT(plugin.registerNode( "stringFormat", stringFormat::id, stringFormat::creator, stringFormat::initialize));
        // Create the UI for the plugin.
        // Keep on going, do not fail the load because of this.
        CHECK_MSTATUS(plugin.registerUI("stringFormatCreateUI", "stringFormatDeleteUI", "", ""));
        return status;
}
// The unitializePlugin is called when Maya needs to unload the plugin.
// It does the opposite of initialize.
//
MStatus uninitializePlugin( MObject obj)
{
        MStatus   status;
        MFnPlugin plugin( obj );
        CHECK_MSTATUS_AND_RETURN_IT(plugin.deregisterNode( stringFormat::id ));
        return status;
}
