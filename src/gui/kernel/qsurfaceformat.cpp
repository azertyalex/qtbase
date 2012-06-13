/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qsurfaceformat.h"

#include <QtCore/qatomic.h>
#include <QtCore/QDebug>

#ifdef major
#undef major
#endif

#ifdef minor
#undef minor
#endif

QT_BEGIN_NAMESPACE

class QSurfaceFormatPrivate
{
public:
    explicit QSurfaceFormatPrivate(QSurfaceFormat::FormatOptions _opts = 0)
        : ref(1)
        , opts(_opts)
        , redBufferSize(-1)
        , greenBufferSize(-1)
        , blueBufferSize(-1)
        , alphaBufferSize(-1)
        , depthSize(-1)
        , stencilSize(-1)
        , swapBehavior(QSurfaceFormat::DefaultSwapBehavior)
        , numSamples(-1)
        , renderableType(QSurfaceFormat::DefaultRenderableType)
        , profile(QSurfaceFormat::NoProfile)
        , major(2)
        , minor(0)
    {
    }

    QSurfaceFormatPrivate(const QSurfaceFormatPrivate *other)
        : ref(1),
          opts(other->opts),
          redBufferSize(other->redBufferSize),
          greenBufferSize(other->greenBufferSize),
          blueBufferSize(other->blueBufferSize),
          alphaBufferSize(other->alphaBufferSize),
          depthSize(other->depthSize),
          stencilSize(other->stencilSize),
          swapBehavior(other->swapBehavior),
          numSamples(other->numSamples),
          renderableType(other->renderableType),
          profile(other->profile),
          major(other->major),
          minor(other->minor)
    {
    }

    QAtomicInt ref;
    QSurfaceFormat::FormatOptions opts;
    int redBufferSize;
    int greenBufferSize;
    int blueBufferSize;
    int alphaBufferSize;
    int depthSize;
    int stencilSize;
    QSurfaceFormat::SwapBehavior swapBehavior;
    int numSamples;
    QSurfaceFormat::RenderableType renderableType;
    QSurfaceFormat::OpenGLContextProfile profile;
    int major;
    int minor;
};

/*!
    \class QSurfaceFormat
    \since 5.0
    \brief The QSurfaceFormat class represents the format of a QSurface.
    \inmodule QtGui

    The format includes the size of the color buffers, red, green, and blue;
    the size of the alpha buffer; the size of the depth and stencil buffers;
    and number of samples per pixel for multisampling. In addition, the format
    contains surface configuration parameters such as OpenGL profile and
    version for rendering, whether or not enable stereo buffers, and swap
    behaviour.
*/

/*!
    \enum QSurfaceFormat::FormatOption

    This enum contains format options for use with QSurfaceFormat.

    \value StereoBuffers Used to request stereo buffers in the surface format.
    \value DebugContext Used to request a debug context with extra debugging information.
        This requires OpenGL version 3.0 or higher.
    \value DeprecatedFunctions Used to request that deprecated functions be included
        in the OpenGL context profile. If not specified, you should get a forward compatible context
        without support functionality marked as deprecated. This requires OpenGL version 3.0 or higher.
*/

/*!
    \enum QSurfaceFormat::SwapBehavior

    This enum is used by QSurfaceFormat to specify the swap behaviour of a surface. The swap behaviour
    is mostly transparent to the application, but it affects factors such as rendering latency and
    throughput.

    \value DefaultSwapBehavior The default, unspecified swap behaviour of the platform.
    \value SingleBuffer Used to request single buffering, which might result in flickering
        when OpenGL rendering is done directly to screen without an intermediate offscreen
        buffer.
    \value DoubleBuffer This is typically the default swap behaviour on desktop platforms,
        consisting of one back buffer and one front buffer. Rendering is done to the back
        buffer, and then the back buffer and front buffer are swapped, or the contents of
        the back buffer are copied to the front buffer, depending on the implementation.
    \value TripleBuffer This swap behaviour is sometimes used in order to decrease the
        risk of skipping a frame when the rendering rate is just barely keeping up with
        the screen refresh rate. Depending on the platform it might also lead to slightly
        more efficient use of the GPU due to improved pipelining behaviour. Triple buffering
        comes at the cost of an extra frame of memory usage and latency, and might not be
        supported depending on the underlying platform.
*/

/*!
    \enum QSurfaceFormat::OpenGLContextProfile

    This enum is used to specify the OpenGL context profile, in
    conjunction with QSurfaceFormat::setMajorVersion() and
    QSurfaceFormat::setMinorVersion().

    Profiles are exposed in OpenGL 3.2 and above, and are used
    to choose between a restricted core profile, and a compatibility
    profile which might contain deprecated support functionality.

    Note that the core profile might still contain functionality that
    is deprecated and scheduled for removal in a higher version. To
    get access to the deprecated functionality for the core profile
    in the set OpenGL version you can use the QSurfaceFormat format option
    QSurfaceFormat::DeprecatedFunctions.

    \value NoProfile            OpenGL version is lower than 3.2.
    \value CoreProfile          Functionality deprecated in OpenGL version 3.0 is not available.
    \value CompatibilityProfile Functionality from earlier OpenGL versions is available.
*/

/*!
    Constructs a default initialized QSurfaceFormat.
*/
QSurfaceFormat::QSurfaceFormat() : d(new QSurfaceFormatPrivate)
{
}

/*!
    Constructs a QSurfaceFormat with the given format \a options.
*/
QSurfaceFormat::QSurfaceFormat(QSurfaceFormat::FormatOptions options) :
    d(new QSurfaceFormatPrivate(options))
{
}

/*!
    \internal
*/
void QSurfaceFormat::detach()
{
    if (d->ref.load() != 1) {
        QSurfaceFormatPrivate *newd = new QSurfaceFormatPrivate(d);
        if (!d->ref.deref())
            delete d;
        d = newd;
    }
}

/*!
    Constructs a copy of \a other.
*/
QSurfaceFormat::QSurfaceFormat(const QSurfaceFormat &other)
{
    d = other.d;
    d->ref.ref();
}

/*!
    Assigns \a other to this object.
*/
QSurfaceFormat &QSurfaceFormat::operator=(const QSurfaceFormat &other)
{
    if (d != other.d) {
        other.d->ref.ref();
        if (!d->ref.deref())
            delete d;
        d = other.d;
    }
    return *this;
}

/*!
    Destroys the QSurfaceFormat.
*/
QSurfaceFormat::~QSurfaceFormat()
{
    if (!d->ref.deref())
        delete d;
}

/*!
    \fn bool QSurfaceFormat::stereo() const

    Returns true if stereo buffering is enabled; otherwise returns
    false. Stereo buffering is disabled by default.

    \sa setStereo()
*/

/*!
    If \a enable is true enables stereo buffering; otherwise disables
    stereo buffering.

    Stereo buffering is disabled by default.

    Stereo buffering provides extra color buffers to generate left-eye
    and right-eye images.

    \sa stereo()
*/
void QSurfaceFormat::setStereo(bool enable)
{
    QSurfaceFormat::FormatOptions newOptions = d->opts;
    if (enable) {
        newOptions |= QSurfaceFormat::StereoBuffers;
    } else {
        newOptions &= ~QSurfaceFormat::StereoBuffers;
    }
    if (int(newOptions) != int(d->opts)) {
        detach();
        d->opts = newOptions;
    }
}

/*!
    Returns the number of samples per pixel when multisampling is
    enabled. By default, multisampling is disabled.

    \sa setSampleBuffers(), sampleBuffers(), setSamples()
*/
int QSurfaceFormat::samples() const
{
   return d->numSamples;
}

/*!
    Set the preferred number of samples per pixel when multisampling
    is enabled to \a numSamples. By default, multisampling is disabled.

    \sa setSampleBuffers(), sampleBuffers(), samples()
*/
void QSurfaceFormat::setSamples(int numSamples)
{
    if (d->numSamples != numSamples) {
        detach();
        d->numSamples = numSamples;
    }
}

/*!
    Sets the format option to \a opt.

    \sa testOption()
*/
void QSurfaceFormat::setOption(QSurfaceFormat::FormatOptions opt)
{
    const QSurfaceFormat::FormatOptions newOptions = d->opts | opt;
    if (int(newOptions) != int(d->opts)) {
        detach();
        d->opts = newOptions;
    }
}

/*!
    Returns true if format option \a opt is set; otherwise returns false.

    \sa setOption()
*/
bool QSurfaceFormat::testOption(QSurfaceFormat::FormatOptions opt) const
{
    return d->opts & opt;
}

/*!
    Set the minimum depth buffer size to \a size.

    \sa depthBufferSize(), setDepth(), depth()
*/
void QSurfaceFormat::setDepthBufferSize(int size)
{
    if (d->depthSize != size) {
        detach();
        d->depthSize = size;
    }
}

/*!
    Returns the depth buffer size.

    \sa depth(), setDepth(), setDepthBufferSize()
*/
int QSurfaceFormat::depthBufferSize() const
{
   return d->depthSize;
}

/*!
    Set the swap behaviour of the surface.

    The swap behaviour specifies whether single, double, or triple
    buffering is desired. The default, SwapBehavior::DefaultSwapBehavior,
    gives the default swap behavior of the platform.
*/
void QSurfaceFormat::setSwapBehavior(SwapBehavior behavior)
{
    if (d->swapBehavior != behavior) {
        detach();
        d->swapBehavior = behavior;
    }
}

/*!
    Returns the configured swap behaviour.

    \sa setSwapBehavior()
*/
QSurfaceFormat::SwapBehavior QSurfaceFormat::swapBehavior() const
{
    return d->swapBehavior;
}

/*!
    Returns true if the alpha buffer size is greater than zero.

    This means that the surface might be used with per pixel
    translucency effects.
*/
bool QSurfaceFormat::hasAlpha() const
{
    return d->alphaBufferSize > 0;
}

/*!
    Set the preferred stencil buffer size to \a size bits.

    \sa stencilBufferSize(), setStencil(), stencil()
*/
void QSurfaceFormat::setStencilBufferSize(int size)
{
    if (d->stencilSize != size) {
        detach();
        d->stencilSize = size;
    }
}

/*!
    Returns the stencil buffer size in bits.

    \sa stencil(), setStencil(), setStencilBufferSize()
*/
int QSurfaceFormat::stencilBufferSize() const
{
   return d->stencilSize;
}

/*!
    Get the size in bits of the red channel of the color buffer.
*/
int QSurfaceFormat::redBufferSize() const
{
    return d->redBufferSize;
}

/*!
    Get the size in bits of the green channel of the color buffer.
*/
int QSurfaceFormat::greenBufferSize() const
{
    return d->greenBufferSize;
}

/*!
    Get the size in bits of the blue channel of the color buffer.
*/
int QSurfaceFormat::blueBufferSize() const
{
    return d->blueBufferSize;
}

/*!
    Get the size in bits of the alpha channel of the color buffer.
*/
int QSurfaceFormat::alphaBufferSize() const
{
    return d->alphaBufferSize;
}

/*!
    Set the desired size in bits of the red channel of the color buffer.
*/
void QSurfaceFormat::setRedBufferSize(int size)
{
    if (d->redBufferSize != size) {
        detach();
        d->redBufferSize = size;
    }
}

/*!
    Set the desired size in bits of the green channel of the color buffer.
*/
void QSurfaceFormat::setGreenBufferSize(int size)
{
    if (d->greenBufferSize != size) {
        detach();
        d->greenBufferSize = size;
    }
}

/*!
    Set the desired size in bits of the blue channel of the color buffer.
*/
void QSurfaceFormat::setBlueBufferSize(int size)
{
    if (d->blueBufferSize != size) {
        detach();
        d->blueBufferSize = size;
    }
}

/*!
    Set the desired size in bits of the alpha channel of the color buffer.
*/
void QSurfaceFormat::setAlphaBufferSize(int size)
{
    if (d->alphaBufferSize != size) {
        detach();
        d->alphaBufferSize = size;
    }
}

/*!
    Sets the desired renderable type.

    Chooses between desktop OpenGL, OpenGL ES, and OpenVG.
*/
void QSurfaceFormat::setRenderableType(RenderableType type)
{
    if (d->renderableType != type) {
        detach();
        d->renderableType = type;
    }
}

/*!
    Gets the renderable type.

    Chooses between desktop OpenGL, OpenGL ES, and OpenVG.
*/
QSurfaceFormat::RenderableType QSurfaceFormat::renderableType() const
{
    return d->renderableType;
}

/*!
    Sets the desired OpenGL context profile.

    This setting is ignored if the requested OpenGL version is
    less than 3.2.
*/
void QSurfaceFormat::setProfile(OpenGLContextProfile profile)
{
    if (d->profile != profile) {
        detach();
        d->profile = profile;
    }
}

/*!
    Get the configured OpenGL context profile.

    This setting is ignored if the requested OpenGL version is
    less than 3.2.
*/
QSurfaceFormat::OpenGLContextProfile QSurfaceFormat::profile() const
{
    return d->profile;
}

/*!
    Sets the desired major OpenGL version.
*/
void QSurfaceFormat::setMajorVersion(int major)
{
    if (d->major != major) {
        detach();
        d->major = major;
    }
}

/*!
    Returns the major OpenGL version.

    The default version is 2.0.
*/
int QSurfaceFormat::majorVersion() const
{
    return d->major;
}

/*!
    Sets the desired minor OpenGL version.

    The default version is 2.0.
*/
void QSurfaceFormat::setMinorVersion(int minor)
{
    if (d->minor != minor) {
        detach();
        d->minor = minor;
    }
}

/*!
    Returns the minor OpenGL version.
*/
int QSurfaceFormat::minorVersion() const
{
    return d->minor;
}

/*!
    Returns true if all the options of the two QSurfaceFormat objects
    are equal.

    \relates QSurfaceFormat
*/
bool operator==(const QSurfaceFormat& a, const QSurfaceFormat& b)
{
    return (a.d == b.d) || ((int) a.d->opts == (int) b.d->opts
        && a.d->stencilSize == b.d->stencilSize
        && a.d->redBufferSize == b.d->redBufferSize
        && a.d->greenBufferSize == b.d->greenBufferSize
        && a.d->blueBufferSize == b.d->blueBufferSize
        && a.d->alphaBufferSize == b.d->alphaBufferSize
        && a.d->depthSize == b.d->depthSize
        && a.d->numSamples == b.d->numSamples
        && a.d->swapBehavior == b.d->swapBehavior
        && a.d->profile == b.d->profile
        && a.d->major == b.d->major
        && a.d->minor == b.d->minor);
}

/*!
    Returns false if all the options of the two QSurfaceFormat objects
    \a a and \a b are equal; otherwise returns true.

    \relates QSurfaceFormat
*/
bool operator!=(const QSurfaceFormat& a, const QSurfaceFormat& b)
{
    return !(a == b);
}

#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug dbg, const QSurfaceFormat &f)
{
    const QSurfaceFormatPrivate * const d = f.d;

    dbg.nospace() << "QSurfaceFormat("
                  << "version " << d->major << '.' << d->minor
                  << ", options " << d->opts
                  << ", depthBufferSize " << d->depthSize
                  << ", redBufferSize " << d->redBufferSize
                  << ", greenBufferSize " << d->greenBufferSize
                  << ", blueBufferSize " << d->blueBufferSize
                  << ", alphaBufferSize " << d->alphaBufferSize
                  << ", stencilBufferSize " << d->stencilSize
                  << ", samples " << d->numSamples
                  << ", swapBehavior " << d->swapBehavior
                  << ", profile  " << d->profile
                  << ')';

    return dbg.space();
}
#endif

QT_END_NAMESPACE
