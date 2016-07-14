#!/usr/bin/env python

from __future__ import division, print_function

import sys
import os

import qt
import yaml
from astropy.io import fits
from astropy.wcs import WCS
import numpy as N

import curvecontrol

def ds9xpa(img, filename=None):
    """Send data to ds9 via xpa."""
    if filename is None:
        tempfn = '/tmp/temp_xpa_%i.fits' % os.getpid()
    else:
        tempfn = filename
        try:
            os.unlink(filename)
        except OSError:
            pass

    hdu = fits.PrimaryHDU()
    hdu.data = img
    hdus = fits.HDUList([hdu])
    hdus.writeto(tempfn)

    os.system('xpaset ds9 fits < %s' % tempfn)

    if filename is None:
        os.unlink(tempfn)

class ImageContainer:
    def __init__(self, pars):

        self.pars = dict(pars)
        self.images = []
        self.scales = []
        self.radii = []
        self.weights = []

        if 'chop' in pars['image'] and pars['image']['chop']['enable']:
            chop = pars['image']['chop']['range']
        else:
            chop = None

        for d in pars['data']:
            print('Loading', d['filename'])
            with fits.open(d['filename']) as f:
                image = f[0].data
            if chop:
                image = image[chop[1]:chop[3],chop[0]:chop[2]]

            self.images.append(image)

            radii = N.array(d['weightrad'], dtype=N.float64)
            self.radii.append(radii)
            weightvals = N.array(d['weightvals'], dtype=N.float64)
            maxval = round(N.max(weightvals), 2)
            self.scales.append(maxval)
            self.weights.append(weightvals/maxval)

        xc, yc = pars['image']['centre']
        if chop:
            xc -= chop[0]
            yc -= chop[1]

        self.radiiimg = N.fromfunction(
            lambda y, x: N.sqrt((x-xc)**2+(y-yc)**2),
            self.images[0].shape)

    def writeOutputPars(self, filename):

        # update values in parameters
        for i, (r, w, s) in enumerate(zip(self.radii, self.weights, self.scales)):
            self.pars['data'][i]['weightrad'] = r.tolist()
            self.pars['data'][i]['weightvals'] = (w*s).tolist()

        print('pars', repr(self.pars))

        # write to output
        with open(filename, 'w') as fout:
            fout.write( yaml.dump(self.pars) )

    def filterAdd(self):
        """Combine input images using interpolation."""

        out = N.zeros(self.images[0].shape)
        for image, radii, weights, scale in zip(
            self.images, self.radii, self.weights, self.scales):
            if scale > 0:
                print(id(image))
                print('radii', radii)
                print('weights', weights)
                print('scale', scale)

                interpol = N.interp(self.radiiimg, radii, weights*scale)
                out += interpol*image

        maxval = N.max(out[N.isfinite(out)])
        out = out / maxval
        return out

class Window(qt.QWidget):
    def __init__(self, infile):
        qt.QWidget.__init__(self)
        layout = qt.QGridLayout()

        # load parameters
        with open(infile) as f:
            self.pars = pars = yaml.load(f)

        self.images = ImageContainer(pars)

        def getOnChanged(idx):
            def func(vals):
                self.images.radii[idx] = vals[0]
                self.images.weights[idx] = vals[1]
                self.redraw()
            return func

        def getEditChanged(cntrl, idx):
            def func():
                text = cntrl.text()
                try:
                    self.images.scales[idx] = float(text)
                except ValueError:
                    cntrl.setText(str(self.images.scales[idx]))
                    return
                self.redraw()
            return func

        def getCheck(idx, cntrl):
            def func():
                self.images.scales[idx] = -self.images.scales[idx]
                cntrl.setText(str(self.images.scales[idx]))
                self.redraw()
            return func

        for i in range(len(self.images.images)):

            radii = self.images.radii[i]
            weights = self.images.weights[i]
            print('r', radii)
            print('w', weights)

            cntrl = curvecontrol.CurveView([radii, weights])
            cntrl.changed.connect( getOnChanged(i) )
            layout.addWidget(cntrl, i, 0)

            l = qt.QLineEdit()
            l.setText(str(self.images.scales[i]))
            l.editingFinished.connect( getEditChanged(l, i) )
            layout.addWidget(l, i, 1)

            c = qt.QCheckBox()
            c.clicked.connect( getCheck(i, l) )
            layout.addWidget(c, i, 2)

        self.setLayout(layout)
        self.redraw()

    def redraw(self):
        img = self.images.filterAdd()
        ds9xpa(img, filename=self.pars['image']['outfilename'])
        self.images.writeOutputPars('out-pars.yml')

def main():
    filename = sys.argv[1]

    app = qt.QApplication(sys.argv)
    win = Window(filename)
    win.show()
    app.exec_()

if __name__ == '__main__':
    main()
