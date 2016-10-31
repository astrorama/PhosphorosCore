import argparse  # for program options from configuration
import ElementsKernel.Logging as log  # for Elements logging support

from scipy import interpolate
from math import log


def defineSpecificProgramOptions():
    """
    @brief Allows to define the (command line and configuration file) options specific to
    this program

    @details
        See the ElementsProgram documentation for more details.
    @return
        An  ArgumentParser.
    """
    parser = argparse.ArgumentParser()
    parser.add_argument('-rv','--rv-parameter', type=float, default=3.1,
                        help='Rv parameter (default=3.1)')
    parser.add_argument('-of','--output-file', type=str, default="./F99.dat",
                        help='The name and path of the output file (default="./F99.dat")')
    parser.add_argument('-sp','--sampling', type=int, default=1000,
                        help='Number of sample of the curve (default=1000)')
    return parser

def mainMethod(args):
    """
    @brief The "main" method.

    @details
        This method is the entry point to the program. In this sense, it is similar to a main
        (and it is why it is called mainMethod()). The code below contains the calls to the
        different classes created for the first developer's workshop

        See the ElementsProgram documentation for more details.
    """

    max_x_value = 10.
    x_div = args.sampling/max_x_value
    x_offset = 1./x_div

    x=[i/x_div + x_offset for i in range(args.sampling)]
    lambda_=[1e4/x_ for x_ in x]
    kL=[0. for i in range(args.sampling)]

    Rv = args.rv_parameter
    C2 = -0.824 + 4.717/Rv
    C1 = 2.030 - 3.007*C2
    xo = 4.596 #um^-1
    gamma = 0.99 #um^-1
    C3 = 3.23
    C4 = 0.41

    wi = [0,0.377,0.820,1.667,1.828,2.141,2.433,3.704,3.846]
    kLi = [0,0.265,0.829,2.688,3.055,3.806,4.315,6.265,6.591]
    tck = interpolate.splrep(wi, kLi, s=0)


    for l in range(args.sampling):
        if lambda_[l]<=2700:
            D = x[l]**2 / ( (x[l]**2 - xo**2)**2 + (x[l]*gamma)**2)
            F=0.
            if x[l]>=5.9:
                 F = (0.5392 * (x[l]-5.9)**2) + (0.05644 * (x[l]-5.9)**3)
            kL[l] = C1 + C2*x[l] + C3*D + C4*F + Rv
        elif lambda_[l]>2700:
            kL[l] = interpolate.splev(x[l], tck, der=0)


    # Create the output

    f = open(args.output_file, 'w')
    for l in range(args.sampling):
        i= args.sampling-1-l
        f.write(str(lambda_[i])+"  "+ str(kL[i]) + "\n")
    f.close()



