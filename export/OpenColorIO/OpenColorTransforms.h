/*
Copyright (c) 2003-2010 Sony Pictures Imageworks Inc., et al.
All Rights Reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:
* Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in the
  documentation and/or other materials provided with the distribution.
* Neither the name of Sony Pictures Imageworks nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef INCLUDED_OCIO_OPENCOLORTRANSFORMS_H
#define INCLUDED_OCIO_OPENCOLORTRANSFORMS_H

#include "OpenColorTypes.h"

#ifndef OCIO_NAMESPACE_ENTER
#error This header cannot be used directly. Use <OpenColorIO/OpenColorIO.h> instead.
#endif

OCIO_NAMESPACE_ENTER
{

    ///////////////////////////////////////////////////////////////////////////
    //
    // Transforms
    //
    // (Typically only needed when creating and/or manipulating configurations)
    
    
    class Transform
    {
    public:
        virtual ~Transform();
        virtual TransformRcPtr createEditableCopy() const = 0;
        
        virtual TransformDirection getDirection() const = 0;
        virtual void setDirection(TransformDirection dir) = 0;
        
    
    private:
        Transform& operator= (const Transform &);
    };
    
    std::ostream& operator<< (std::ostream&, const Transform&);
    
    
    
    
    
    
    
    
    ///////////////////////////////////////////////////////////////////////////
    
    // An implementation of the ASC CDL Transfer Functions and Interchange
    // Syntax (Based on the version 1.2 document)
    //
    // Note: the clamping portion of the CDL is only applied if 
    // a non-identity power is specified.
    
    class CDLTransform : public Transform
    {
    public:
        static CDLTransformRcPtr Create();
        
        virtual TransformRcPtr createEditableCopy() const;
        
        virtual TransformDirection getDirection() const;
        virtual void setDirection(TransformDirection dir);
        
        bool equals(const ConstCDLTransformRcPtr & other) const;
        
        const char * getXML() const;
        void setXML(const char * xml);
        
        // ASC_SOP
        // Slope, offset, power
        // out = clamp( (in * slope) + offset ) ^ power
        //
        
        void setSlope(const float * rgb);
        void getSlope(float * rgb) const;
        
        void setOffset(const float * rgb);
        void getOffset(float * rgb) const;
        
        void setPower(const float * rgb);
        void getPower(float * rgb) const;
        
        void setSOP(const float * vec9);
        void getSOP(float * vec9) const;
        
        // ASC_SAT
        
        void setSat(float sat);
        float getSat() const;
        
        // These are hard-coded, by spec, to r709
        void getSatLumaCoefs(float * rgb) const;
        
        // Metadata
        // These do not affect the image processing, but
        // are often useful for pipeline purposes and are
        // included in the serialization.
        
        // Unique Identifier for this correction
        void setID(const char * id);
        const char * getID() const;
        
        // Textual description of color correction
        // (stored on the SOP)
        void setDescription(const char * desc);
        const char * getDescription() const;
    
    private:
        CDLTransform();
        CDLTransform(const CDLTransform &);
        virtual ~CDLTransform();
        
        CDLTransform& operator= (const CDLTransform &);
        
        static void deleter(CDLTransform* t);
        
        class Impl;
        friend class Impl;
        std::auto_ptr<Impl> m_impl;
    };
    
    std::ostream& operator<< (std::ostream&, const CDLTransform&);
    
    
    
    ///////////////////////////////////////////////////////////////////////////
    
    
    class ColorSpaceTransform : public Transform
    {
    public:
        static ColorSpaceTransformRcPtr Create();
        
        virtual TransformRcPtr createEditableCopy() const;
        
        virtual TransformDirection getDirection() const;
        virtual void setDirection(TransformDirection dir);
        
        const char * getSrc() const;
        void setSrc(const char * src);
        
        const char * getDst() const;
        void setDst(const char * dst);
    
    private:
        ColorSpaceTransform();
        ColorSpaceTransform(const ColorSpaceTransform &);
        virtual ~ColorSpaceTransform();
        
        ColorSpaceTransform& operator= (const ColorSpaceTransform &);
        
        static void deleter(ColorSpaceTransform* t);
        
        class Impl;
        friend class Impl;
        std::auto_ptr<Impl> m_impl;
    };
    
    std::ostream& operator<< (std::ostream&, const ColorSpaceTransform&);
    
    
    
    
    
    ///////////////////////////////////////////////////////////////////////////
    
    
    
    
    class DisplayTransform : public Transform
    {
    public:
        static DisplayTransformRcPtr Create();
        
        virtual TransformRcPtr createEditableCopy() const;
        
        virtual TransformDirection getDirection() const;
        virtual void setDirection(TransformDirection dir);
        
        
        // Step 0. Specify the incoming color space
        void setInputColorSpaceName(const char * name);
        const char * getInputColorSpaceName() const;
        
        // Step 1: Apply a Color Correction, in ROLE_SCENE_LINEAR
        void setLinearCC(const ConstTransformRcPtr & cc);
        ConstTransformRcPtr getLinearCC() const;
        
        // Step 2: Apply a color correction, in ROLE_COLOR_TIMING
        void setColorTimingCC(const ConstTransformRcPtr & cc);
        ConstTransformRcPtr getColorTimingCC() const;
        
        // Step 3: Apply the Channel Viewing Swizzle (mtx)
        void setChannelView(const ConstTransformRcPtr & transform);
        ConstTransformRcPtr getChannelView() const;
        
        // Step 4: Apply the output display transform
        void setDisplayColorSpaceName(const char * name);
        const char * getDisplayColorSpaceName() const;
    
    private:
        DisplayTransform();
        DisplayTransform(const DisplayTransform &);
        virtual ~DisplayTransform();
        
        DisplayTransform& operator= (const DisplayTransform &);
        
        static void deleter(DisplayTransform* t);
        
        class Impl;
        friend class Impl;
        std::auto_ptr<Impl> m_impl;
    };
    
    std::ostream& operator<< (std::ostream&, const DisplayTransform&);
    
    
    
    
    
    ///////////////////////////////////////////////////////////////////////////
    
    
    class FileTransform : public Transform
    {
    public:
        static FileTransformRcPtr Create();
        
        virtual TransformRcPtr createEditableCopy() const;
        
        virtual TransformDirection getDirection() const;
        virtual void setDirection(TransformDirection dir);
        
        const char * getSrc() const;
        void setSrc(const char * src);
        
        Interpolation getInterpolation() const;
        void setInterpolation(Interpolation interp);
    
    private:
        FileTransform();
        FileTransform(const FileTransform &);
        virtual ~FileTransform();
        
        FileTransform& operator= (const FileTransform &);
        
        static void deleter(FileTransform* t);
        
        class Impl;
        friend class Impl;
        std::auto_ptr<Impl> m_impl;
    };
    
    std::ostream& operator<< (std::ostream&, const FileTransform&);
    
    
    
    ///////////////////////////////////////////////////////////////////////////
    
    
    class GroupTransform : public Transform
    {
    public:
        static GroupTransformRcPtr Create();
        
        virtual TransformRcPtr createEditableCopy() const;
        
        virtual TransformDirection getDirection() const;
        virtual void setDirection(TransformDirection dir);
        
        ConstTransformRcPtr getTransform(int index) const;
        TransformRcPtr getEditableTransform(int index);
        
        int size() const;
        void push_back(const ConstTransformRcPtr& transform);
        void clear();
        bool empty() const;
    
    private:
        GroupTransform();
        GroupTransform(const GroupTransform &);
        virtual ~GroupTransform();
        
        GroupTransform& operator= (const GroupTransform &);
        
        static void deleter(GroupTransform* t);
        
        class Impl;
        friend class Impl;
        std::auto_ptr<Impl> m_impl;
    };
    
    std::ostream& operator<< (std::ostream&, const GroupTransform&);
    
    
    
    ///////////////////////////////////////////////////////////////////////////
    //
    // Represents an MX+B Matrix transform
    
    class MatrixTransform : public Transform
    {
    public:
        static MatrixTransformRcPtr Create();
        
        virtual TransformRcPtr createEditableCopy() const;
        
        virtual TransformDirection getDirection() const;
        virtual void setDirection(TransformDirection dir);
        
        bool equals(const MatrixTransform & other) const;
        
        void setValue(const float * m44, const float * offset4);
        void getValue(float * m44, float * offset4) const;
        
        
        // Convenience functions to get the mtx and offset
        // corresponding to higher-level concepts
        
        // This can throw an exception if for any component
        // oldmin == oldmax. (divide by 0)
        
        static void Fit(float * m44, float * offset4,
                        const float * oldmin4, const float * oldmax4,
                        const float * newmin4, const float * newmax4);
        
        static void Identity(float * m44, float * offset4);
        
        static void Sat(float * m44, float * offset4,
                        float sat, const float * lumaCoef3);
        
        static void Scale(float * m44, float * offset4,
                          const float * scale4);
        
        static void View(float * m44, float * offset4,
                         int * channelHot4,
                         const float * lumaCoef3);
    
    private:
        MatrixTransform();
        MatrixTransform(const MatrixTransform &);
        virtual ~MatrixTransform();
        
        MatrixTransform& operator= (const MatrixTransform &);
        
        static void deleter(MatrixTransform* t);
        
        class Impl;
        friend class Impl;
        std::auto_ptr<Impl> m_impl;
    };
    
    std::ostream& operator<< (std::ostream&, const MatrixTransform&);
    
}
OCIO_NAMESPACE_EXIT

#endif
