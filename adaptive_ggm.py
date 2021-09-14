#!/usr/bin/env python3

import os.path
import argparse
import subprocess

from astropy.io import fits
import numpy as np

def _escapeArgs(args):
    """Escape arguments for printing."""
    out = []
    for a in args:
        a = a.replace("'", r"\'")
        wrap = False
        for c in ' \t[]<>|()$"#!*?~&;':
            if c in a:
                wrap = True
                break
        if wrap:
            a = "'%s'" % a
        out.append(a)
    return ' '.join(out)

def call(*args, **argsv):
    """Shortcut for running external programs."""
    if not argsv.get('silent', False):
        print("+", _escapeArgs(args))
    subprocess.check_call(args)

def calcGradient(inimg, log):
    """Calculate gradient magnitude of input image."""

    if log:
        inimg = np.log10(inimg)

    gy = inimg[1:,:]-inimg[:-1,:]
    gx = inimg[:,1:]-inimg[:,:-1]
    gyc = 0.5*(gy[1:,:]+gy[:-1,:])
    gxc = 0.5*(gx[:,1:]+gx[:,:-1])

    gyc = np.pad(gyc, ((1,1),(0,0)))
    gxc = np.pad(gxc, ((0,0),(1,1)))
    grad = np.sqrt(gxc**2 + gyc**2)

    return grad

def main():

    parser = argparse.ArgumentParser(
        description="Adapive Gaussian gradient magnitude",
    formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument('counts', help='counts image filename')

    parser.add_argument('--image', help='optional image filename to apply smoothing to (uses counts if not set)')
    parser.add_argument('--sn', type=float, default=32, help='Smoothing S/N ratio')
    parser.add_argument('--log', type=bool, default=True, help='Calculate log after smoothing')
    parser.add_argument('--mask', help='Mask image (optional)')
    parser.add_argument('--scale', help='Intermediate scale map filename', default='scale.fits')
    parser.add_argument('--smimage', help='Intermediate smoothed image filename', default='smoothed.fits')
    parser.add_argument('--threads', type=int, default=4, help='Number of threads to use when smoothing')
    parser.add_argument('--contbin-dir', help='Override location of contour binning code')

    parser.add_argument('output', help='output image filename')
    args = parser.parse_args()

    # where to find program
    program = 'accumulate_counts'
    if args.contbin_dir:
        program = os.path.join(args.contbin_dir, program)

    # first calculate scale map
    print('* Calculating scale map')
    cargs = [
        args.counts,
        '--scale=%s' % args.scale,
        '--sn=%g' % args.sn,
        '--threads=%i' % args.threads,
    ]
    if args.mask:
        cargs.append('--mask=%s' % args.mask)

    call(program, *cargs)

    # now smooth input image (using input image of counts image)
    print('* Smoothing input image')
    inimage = args.image or args.counts

    cargs = [
        inimage,
        '--apply',
        '--scale=%s' % args.scale,
        '--applied=%s' % args.smimage,
        '--threads=%i' % args.threads,
        '--gaussian',
    ]
    if args.mask:
        cargs.append('--mask=%s' % args.mask)

    call(program, *cargs)

    # get gradient
    print('* Calculating gradient')
    smf = fits.open(args.smimage, 'readonly')
    smimg = smf[0].data
    grad = calcGradient(smimg, args.log)
    smf.close()
    
    # write to output filename (based on input image)
    inf = fits.open(inimage, 'readonly')
    inf[0].data = grad
    print('* Writing gradient image', args.output)
    inf.writeto(args.output, overwrite=True)

if __name__ == '__main__':
    main()
